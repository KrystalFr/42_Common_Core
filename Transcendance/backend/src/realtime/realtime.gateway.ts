import { ConnectedSocket, MessageBody, OnGatewayConnection, OnGatewayDisconnect, SubscribeMessage, WebSocketGateway, WebSocketServer } from '@nestjs/websockets';
import { BadRequestException, Injectable, Logger, NotFoundException, OnModuleDestroy, OnModuleInit, UnauthorizedException } from '@nestjs/common';
import { JwtService } from '@nestjs/jwt';
import { Server, Socket } from 'socket.io';
import { PrismaService } from '../prisma/prisma.service.js';
import { RoomPresenceService } from '../rooms/room-presence.service.js';
import { RoomsService } from '../rooms/rooms.service.js';
import { ChatService } from '../chat/chat.service.js';
import { AiService } from '../ai/ai.service.js';
import { BotsService } from './bots.service.js';
import { StatsService } from '../stats/stats.service.js';
import { LedgerService } from '../ledger/ledger.service.js';
import { GameService } from './game.service.js';
import { CUID_PATTERN } from '../common/pipes/cuid-param.pipe.js';
import { RealtimeEvents } from './realtime.events.js';

export type AuthenticatedSocket = Socket & { data: { userId: string; email?: string; roomId?: string } };
interface RoomPayload { roomId: string }
type UserProfilePayload = { id: string; email: string; displayName: string | null; avatarUrl: string | null; status: string; createdAt: Date; updatedAt: Date; };

@WebSocketGateway({ path: '/socket.io', transports: ['websocket', 'polling'], cors: { origin: false } })
@Injectable()
export class RealtimeGateway implements OnGatewayConnection, OnGatewayDisconnect, OnModuleInit, OnModuleDestroy {
  private readonly logger = new Logger(RealtimeGateway.name);
  private readonly socketsByUser = new Map<string, Set<string>>();
  private battement?: ReturnType<typeof setInterval>;
  private readonly finishedRoomTimers = new Map<string, NodeJS.Timeout>();
  private static readonly BATTEMENT_MS = 20_000;
  private static readonly FINISHED_ROOM_TIMEOUT_MS = 60_000;

  @WebSocketServer() server!: Server;

  constructor(
    private readonly jwtService: JwtService,
    private readonly prisma: PrismaService,
    private readonly presence: RoomPresenceService,
    private readonly rooms: RoomsService,
    private readonly chat: ChatService,
    private readonly ai: AiService,
    private readonly bots: BotsService,
    private readonly stats: StatsService,
    private readonly ledger: LedgerService,
    private readonly games: GameService,
    private readonly realtimeEvents: RealtimeEvents,
  ) {}

  onModuleInit() { this.realtimeEvents.attach(this.server); this.battement = setInterval(() => void this.refreshPresence(), RealtimeGateway.BATTEMENT_MS); }
  onModuleDestroy() {
    if (this.battement) clearInterval(this.battement);
    for (const timer of this.finishedRoomTimers.values()) clearTimeout(timer);
    this.finishedRoomTimers.clear();
  }

  async handleConnection(raw: Socket) {
    const socket = raw as AuthenticatedSocket;
    try {
      const token = socket.handshake.auth?.token || (typeof socket.handshake.headers.authorization === 'string' ? socket.handshake.headers.authorization.replace(/^Bearer\s+/i, '') : undefined);
      if (!token) throw new UnauthorizedException('Authentification requise');
      const payload = this.jwtService.verify<{ sub?: string }>(token);
      if (!payload.sub) throw new UnauthorizedException('Jeton invalide');
      const user = await this.prisma.user.findUnique({ where: { id: payload.sub }, select: { id: true, email: true } });
      if (!user) throw new UnauthorizedException('Utilisateur introuvable');
      socket.data.userId = user.id; socket.data.email = user.email;
      this.addSocket(user.id, socket.id);
      await socket.join(this.userChannel(user.id));
      await this.syncUserPresence(user.id);
      socket.emit('connection:ready', { userId: user.id, socketId: socket.id });
      const roomId = socket.handshake.auth?.roomId;
      if (typeof roomId === 'string' && roomId) {
        try { await this.joinRoom(socket, { roomId }); }
        catch (error) { socket.emit('room:error', { message: error instanceof Error ? error.message : 'Impossible de rejoindre le salon' }); }
      }
    } catch (error) {
      socket.emit('error', { code: 'UNAUTHORIZED', message: 'Authentification requise' });
      socket.disconnect(true);
    }
  }

  async handleDisconnect(raw: Socket) {
    const socket = raw as AuthenticatedSocket;
    const userId = socket.data.userId;
    if (!userId) return;
    this.removeSocket(userId, socket.id);
    const left = await this.presence.leave(socket.id);
    if (left?.leftRoom) {
      const room = await this.prisma.room.findUnique({ where: { id: left.roomId }, select: { status: true } }).catch(() => null);
      if (room?.status === 'WAITING') {
        await this.rooms.setNotReadyOnDisconnect(left.roomId, userId);
        await this.rooms.resetLobbyTimer(left.roomId);
      }
      this.server?.to(this.roomChannel(left.roomId)).emit('room:member_disconnected', { userId });
      await this.safeBroadcastState(left.roomId);
    }
    await this.syncUserPresence(userId);
  }

  @SubscribeMessage('room:join')
  async joinRoom(@ConnectedSocket() socket: AuthenticatedSocket, @MessageBody() body: RoomPayload) {
    this.assertCuid(body?.roomId, 'roomId');
    const duplicate = [...(this.socketsByUser.get(socket.data.userId) ?? [])].some((socketId) => {
      if (socketId === socket.id) return false;
      const other = this.server?.sockets.sockets.get(socketId) as AuthenticatedSocket | undefined;
      return other?.connected && other.data.roomId === body.roomId;
    });
    if (duplicate) throw new BadRequestException('Une autre connexion est déjà active dans ce salon');
    const room = await this.rooms.findOne(body.roomId);
    await this.rooms.join(body.roomId, socket.data.userId);
    socket.data.roomId = body.roomId;
    await socket.join(this.roomChannel(body.roomId));
    await this.presence.join(socket.id, socket.data.userId, body.roomId);
    await this.syncUserPresence(socket.data.userId, true);
    const state = await this.rooms.getState(body.roomId);
    socket.emit('room:joined', state);
    socket.to(this.roomChannel(body.roomId)).emit('room:member_joined', { userId: socket.data.userId });
    this.server.to(this.roomChannel(body.roomId)).emit('room:state', state);
    if (room.status === 'WAITING') this.broadcastRoomListUpdate();
    return state;
  }

  @SubscribeMessage('room:ready')
  async ready(@ConnectedSocket() socket: AuthenticatedSocket, @MessageBody() body: RoomPayload) {
    await this.assertInRoom(socket, body.roomId);
    const member = await this.rooms.setReady(body.roomId, socket.data.userId);
    await this.publishRoomState(body.roomId, 'room:ready');
    return member;
  }

  @SubscribeMessage('room:kick')
  async kick(@ConnectedSocket() socket: AuthenticatedSocket, @MessageBody() body: RoomPayload & { targetUserId: string }) {
    await this.assertInRoom(socket, body.roomId);
    this.assertCuid(body.targetUserId, 'targetUserId');
    const result = await this.rooms.kick(body.roomId, socket.data.userId, body.targetUserId);
    this.server.to(this.roomChannel(body.roomId)).emit('room:kicked', { userId: body.targetUserId, reason: 'lobby' });
    await this.ejectUserFromRoom(body.roomId, body.targetUserId);
    await this.publishRoomState(body.roomId, 'room:state');
    this.broadcastRoomListUpdate();
    return result;
  }

  @SubscribeMessage('room:leave')
  async leave(@ConnectedSocket() socket: AuthenticatedSocket, @MessageBody() body?: RoomPayload) {
    const roomId = body?.roomId || socket.data.roomId;
    if (!roomId) return { roomId: null };
    await this.assertInRoom(socket, roomId);
    const room = await this.rooms.findOne(roomId);
    let gameLeave = room.status === 'IN_PROGRESS';

    if (gameLeave) {
      const leaveResult = await this.games.leaveGame(roomId, socket.data.userId);
      await this.broadcastBalanceUpdate(socket.data.userId);
      if (leaveResult?.cancelledUserId) {
        this.server?.to(this.roomChannel(roomId)).emit('game:cancelled', {
          roomId,
          reason: 'single_player',
        });
        await this.broadcastBalanceUpdate(leaveResult.cancelledUserId);
      }
    } else {
      const leftLobby = await this.rooms.leaveLobby(roomId, socket.data.userId);
      if (!leftLobby) {
        const currentRoom = await this.rooms.findOne(roomId);
        gameLeave = currentRoom.status === 'IN_PROGRESS';
        if (gameLeave) {
          const leaveResult = await this.games.leaveGame(roomId, socket.data.userId);
          await this.broadcastBalanceUpdate(socket.data.userId);
          if (leaveResult?.cancelledUserId) {
            this.server?.to(this.roomChannel(roomId)).emit('game:cancelled', {
              roomId,
              reason: 'single_player',
            });
            await this.broadcastBalanceUpdate(leaveResult.cancelledUserId);
          }
        }
      }
    }
    const destroyed = gameLeave
      ? await this.rooms.destroyRoomIfEmpty(roomId)
      : false;
    await socket.leave(this.roomChannel(roomId));
    await this.presence.leave(socket.id);
    delete socket.data.roomId;
    await this.syncUserPresence(socket.data.userId, true);
    if (!destroyed) {
      this.server.to(this.roomChannel(roomId)).emit('room:member_left', { userId: socket.data.userId });
      await this.safeBroadcastState(roomId);
    }
    this.broadcastRoomListUpdate();
    return { roomId };
  }

  @SubscribeMessage('room:state')
  async state(@MessageBody() body: RoomPayload) { return this.rooms.getState(body.roomId); }

  @SubscribeMessage('bankrupt:claim')
  async claimRecovery(@ConnectedSocket() socket: AuthenticatedSocket) {
    const result = await this.games.claimRecovery(socket.data.userId);
    await this.broadcastBalanceUpdate(socket.data.userId);
    return result;
  }

  @SubscribeMessage('bet:confirm')
  async confirmBet(@ConnectedSocket() socket: AuthenticatedSocket, @MessageBody() body: { roomId: string; roundId: string; stake: number }) {
    await this.assertInRoom(socket, body.roomId); this.assertCuid(body.roundId, 'roundId'); this.assertPositiveInteger(body.stake, 'stake');
    const bet = await this.games.confirmBet(body.roundId, socket.data.userId, body.stake);
    await this.publishRoomState(body.roomId, 'bet:confirmed', body.roundId);
    return bet;
  }

  @SubscribeMessage('answer:submit')
  async answer(@ConnectedSocket() socket: AuthenticatedSocket, @MessageBody() body: { roomId: string; roundId: string; answer: 'FACT' | 'FAKE' }) {
    await this.assertInRoom(socket, body.roomId); this.assertCuid(body.roundId, 'roundId');
    if (body.answer !== 'FACT' && body.answer !== 'FAKE') throw new BadRequestException('Réponse invalide');
    const bet = await this.games.answer(body.roundId, socket.data.userId, body.answer);
    await this.broadcastBalanceUpdate(socket.data.userId);
    await this.publishRoomState(body.roomId, 'answer:submitted', body.roundId);
    return bet;
  }

  @SubscribeMessage('round:video-unavailable')
  async videoUnavailable(@ConnectedSocket() socket: AuthenticatedSocket, @MessageBody() body: RoomPayload & { roundId: string }) {
    await this.assertInRoom(socket, body.roomId);
    this.assertCuid(body.roundId, 'roundId');
    const cancelled = await this.games.cancelRoundForUnavailableVideo(body.roundId, socket.data.userId);
    if (!cancelled) return { cancelled: false };
    this.server?.to(this.roomChannel(body.roomId)).emit('round:cancelled', { roundId: body.roundId, reason: 'video_unavailable' });
    const round = await this.prisma.round.findUnique({ where: { id: body.roundId }, select: { gameId: true } });
    if (round?.gameId) {
      const next = await this.games.advanceAfterCancelledRound(round.gameId, body.roundId);
      if (next?.finished) {
        await this.publishRoomState(body.roomId, 'game:finished');
        const game = await this.prisma.game.findUnique({ where: { id: round.gameId }, select: { finishedAt: true } });
        this.scheduleFinishedRoomCleanup(body.roomId, game?.finishedAt);
      } else if (next) {
        await this.publishRoomState(body.roomId, 'round:started', next.roundId);
      }
    }
    return { cancelled: true };
  }

  @SubscribeMessage('clip:report')
  async report(@ConnectedSocket() socket: AuthenticatedSocket, @MessageBody() body: RoomPayload & { claimId: string; reason?: string }) {
    await this.assertInRoom(socket, body.roomId); this.assertCuid(body.claimId, 'claimId');
    if (body.reason !== undefined && (typeof body.reason !== 'string' || body.reason.length > 300)) throw new BadRequestException('Motif invalide');
    await this.prisma.clipReport.upsert({ where: { userId_claimId: { userId: socket.data.userId, claimId: body.claimId } }, update: { reason: body.reason ?? null }, create: { userId: socket.data.userId, claimId: body.claimId, reason: body.reason ?? null } });
    return { signale: true };
  }

  @SubscribeMessage('chat:send')
  async sendChat(@ConnectedSocket() socket: AuthenticatedSocket, @MessageBody() body: RoomPayload & { content: string }) {
    await this.assertInRoom(socket, body.roomId);
    if (typeof body.content !== 'string' || body.content.length === 0 || body.content.length > 1000) throw new BadRequestException('Contenu invalide');
    const prompt = body.content.replace(/^\s*(?:@bot|\/ask)\s*/i, '').trim();
    const toBot = prompt !== body.content.trim() && prompt.length > 0;
    const message = await this.chat.sendMessage(socket.data.userId, body.roomId, body.content);
    if (message.action === 'delete') { socket.emit('chat:moderated', { reason: 'Message bloqué par la modération.' }); return message; }
    this.server.to(this.roomChannel(body.roomId)).emit('chat:message', message);
    if (toBot) {
      for await (const event of this.chat.streamBot(socket.data.userId, body.roomId, prompt)) socket.emit(`ai:${event.type}`, event.data);
      return { action: 'bot' };
    }
    void this.bots.interpeller(body.roomId, body.content, (m) => this.diffuserMessageBot(body.roomId, m), socket.data.email?.split('@')[0]);
    return message;
  }

  @SubscribeMessage('ai:chat')
  async aiChat(@ConnectedSocket() socket: AuthenticatedSocket, @MessageBody() body: { prompt?: string; roomId?: string }) {
    if (typeof body?.prompt !== 'string' || body.prompt.trim().length === 0 || body.prompt.length > 4000) throw new BadRequestException('Invite invalide');
    if (body.roomId) await this.assertInRoom(socket, body.roomId);
    for await (const event of this.ai.streamRag(body.prompt, socket.data.userId, { claims: [], exclureUrls: [] })) socket.emit(`ai:${event.type}`, event.data);
    return { success: true };
  }

  diffuserMessageBot(roomId: string, message: { author: string; content: string }) {
    this.server.to(this.roomChannel(roomId)).emit('chat:message', { action: 'allow', message: { id: `bot-${Date.now()}-${Math.random().toString(36).slice(2, 8)}`, content: message.content, createdAt: new Date().toISOString(), user: { email: `${message.author}@factarena.local` } } });
  }

  async publishRoomState(roomId: string, event = 'room:state', roundId?: string) {
    const state = await this.rooms.getState(roomId, roundId);
    this.server?.to(this.roomChannel(roomId)).emit(event, state);
    if (event !== 'room:state') this.server?.to(this.roomChannel(roomId)).emit('room:state', state);
    return state;
  }

  async publishRoundState(roomId: string, event: string, roundId?: string) { return this.publishRoomState(roomId, event, roundId); }

  publishRoundVerdict(roomId: string, roundId: string, truth: string, total: number, good: number) {
    this.server?.to(this.roomChannel(roomId)).emit('round:verdict', { roundId, truth, total, good });
  }

  async broadcastBalanceUpdate(userId: string) {
    const balance = await this.ledger.getBalance(userId);
    for (const id of this.socketsByUser.get(userId) ?? []) {
      this.server?.to(id).emit('balance:update', {
        userId,
        virtualBalance: balance.virtualBalance,
        recoveryAvailable: balance.recoveryAvailable,
      });
    }
  }

  broadcastRoomListUpdate() { this.server?.emit('rooms:update'); }

  invalidateStats(userId: string) { this.server?.emit('stats:invalidate', { userId }); }
  invalidateHistory(userId: string) { this.server?.emit('history:invalidate', { userId }); }
  invalidateLeaderboard() { this.server?.emit('leaderboard:invalidate'); }

  async broadcastProfileUpdate(user: UserProfilePayload) {
    const friendIds = await this.getAcceptedFriendIds(user.id);
    const recipients = new Set([user.id, ...friendIds]);
    for (const recipientId of recipients) {
      this.server?.to(this.userChannel(recipientId)).emit('profile:update', user);
    }
  }

  async notifyFriendUpdate(
    userId: string,
    event: 'friend:request-received' | 'friend:request-accepted' | 'friend:request-rejected' | 'friend:removed',
    payload: { requestId?: string; friendId?: string } = {},
  ) {
    this.server?.to(this.userChannel(userId)).emit(event, payload);
  }

  async syncUserPresence(userId: string, forceNotify = false) {
    const currentRoomId = await this.presence.getCurrentRoomId(userId);
    const connected = (this.socketsByUser.get(userId)?.size ?? 0) > 0;
    const status = currentRoomId ? 'IN_GAME' : connected ? 'ONLINE' : 'OFFLINE';

    const updated = await this.prisma.user.updateMany({
      where: { id: userId, ...(forceNotify ? {} : { status: { not: status } }) },
      data: { status },
    });
    if (updated.count === 0 && !forceNotify) return;

    const friendIds = await this.getAcceptedFriendIds(userId);
    const payload = { userId, status, currentRoomId };
    for (const friendId of friendIds) {
      this.server?.to(this.userChannel(friendId)).emit('presence:update', payload);
    }
  }

  private async getAcceptedFriendIds(userId: string) {
    const relations = await this.prisma.friend.findMany({
      where: {
        status: 'ACCEPTED',
        OR: [{ userId }, { friendId: userId }],
      },
      select: { userId: true, friendId: true },
    });
    return relations.map((relation) => relation.userId === userId ? relation.friendId : relation.userId);
  }

  scheduleFinishedRoomCleanup(roomId: string, finishedAt?: Date | string | null) {
    const existing = this.finishedRoomTimers.get(roomId);
    if (existing) clearTimeout(existing);

    const at = finishedAt ? new Date(finishedAt).getTime() : Date.now();
    const delay = Math.max(0, at + RealtimeGateway.FINISHED_ROOM_TIMEOUT_MS - Date.now());
    const timer = setTimeout(() => {
      this.finishedRoomTimers.delete(roomId);
      void this.closeFinishedRoom(roomId);
    }, delay);
    this.finishedRoomTimers.set(roomId, timer);
  }

  async closeFinishedRoom(roomId: string) {
    const userIds = await this.rooms.closeFinishedRoom(roomId);
    if (!userIds.length) {
      await this.rooms.destroyRoomIfEmpty(roomId);
      return false;
    }

    this.server?.to(this.roomChannel(roomId)).emit('room:kicked', {
      userIds,
      reason: 'finished_timeout',
    });

    for (const userId of userIds) {
      await this.ejectUserFromRoom(roomId, userId);
    }

    const destroyed = await this.rooms.destroyRoomIfEmpty(roomId);
    this.finishedRoomTimers.delete(roomId);
    return destroyed;
  }

  async ejectRoom(roomId: string) {
    for (const raw of this.server?.sockets.sockets.values() ?? []) {
      const socket = raw as AuthenticatedSocket;
      if (socket.data.roomId !== roomId) continue;
      await socket.leave(this.roomChannel(roomId));
      delete socket.data.roomId;
      await this.presence.leave(socket.id);
    }
    await this.presence.clearRoom(roomId);
    for (const userId of this.socketsByUser.keys()) {
      await this.syncUserPresence(userId, true);
    }
  }

  async ejectUserFromRoom(roomId: string, userId: string) {
    for (const socketId of this.socketsByUser.get(userId) ?? []) {
      const socket = this.server?.sockets.sockets.get(socketId) as AuthenticatedSocket | undefined;
      if (!socket || socket.data.roomId !== roomId) continue;
      await socket.leave(this.roomChannel(roomId));
      delete socket.data.roomId;
      await this.presence.leave(socket.id);
    }
    await this.syncUserPresence(userId, true);
  }

  private async refreshPresence() {
    const entries = [...(this.server?.sockets.sockets.values() ?? [])].map((s) => s as AuthenticatedSocket).filter((s) => s.data?.userId && s.data?.roomId).map((s) => ({ socketId: s.id, userId: s.data.userId, roomId: s.data.roomId as string }));
    if (entries.length) await this.presence.rafraichir(entries);
  }
  private async safeBroadcastState(roomId: string) { try { await this.publishRoomState(roomId); } catch {  } }
  private async assertInRoom(socket: AuthenticatedSocket, roomId: string) {
    if (!roomId || socket.data.roomId !== roomId) throw new UnauthorizedException('Rejoignez d’abord le salon');
    const member = await this.prisma.roomMember.findUnique({ where: { roomId_userId: { roomId, userId: socket.data.userId } }, select: { id: true } });
    if (!member) throw new UnauthorizedException('Vous ne faites plus partie du salon');
  }
  private addSocket(userId: string, socketId: string) { const set = this.socketsByUser.get(userId) ?? new Set<string>(); set.add(socketId); this.socketsByUser.set(userId, set); }
  private removeSocket(userId: string, socketId: string) { const set = this.socketsByUser.get(userId); if (!set) return; set.delete(socketId); if (!set.size) this.socketsByUser.delete(userId); }
  private roomChannel(id: string) { return `room:${id}`; }
  private userChannel(id: string) { return `user:${id}`; }
  private assertCuid(value: unknown, name: string) { if (typeof value !== 'string' || !CUID_PATTERN.test(value)) throw new BadRequestException(`${name} invalide`); }
  private assertPositiveInteger(value: unknown, name: string) { if (!Number.isSafeInteger(value) || Number(value) <= 0) throw new BadRequestException(`${name} invalide`); }
}
