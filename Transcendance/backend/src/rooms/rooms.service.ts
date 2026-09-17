import { BadRequestException, ConflictException, Injectable, NotFoundException } from '@nestjs/common';
import { Prisma } from '@prisma/client';
import { PrismaService } from '../prisma/prisma.service.js';
import { ChatService } from '../chat/chat.service.js';
import { RoomPresenceService } from './room-presence.service.js';
import { RealtimeEvents } from '../realtime/realtime.events.js';
import { RESULT_DISPLAY_MS } from '../realtime/game-rules.js';

@Injectable()
export class RoomsService {
  static readonly LOBBY_TIMEOUT_MS = 90_000;
  static readonly KICK_AFTER_MS = 45_000;

  constructor(
    private readonly prisma: PrismaService,
    private readonly chatService: ChatService,
    private readonly roomPresence: RoomPresenceService,
    private readonly realtimeEvents: RealtimeEvents,
  ) {}

  async create(data: { name: string }, createdBy: string) {
    for (let attempt = 0; attempt < 3; attempt += 1) {
      try {
        const expires = new Date(Date.now() + RoomsService.LOBBY_TIMEOUT_MS);
        const room = await this.prisma.$transaction(async (tx) => {
          await this.lockUser(tx, createdBy);
          await this.reserveActiveRoom(tx, createdBy, null);

          const now = new Date();
          const availableCount = await tx.room.count({
            where: {
              status: 'WAITING',
              OR: [{ lobbyExpiresAt: null }, { lobbyExpiresAt: { gt: now } }],
            },
          });
          if (availableCount >= 5) throw new BadRequestException('Le nombre maximum de rooms disponibles (5) est atteint');

          const created = await tx.room.create({
            data: { name: data.name, createdBy, status: 'WAITING', lobbyExpiresAt: expires },
          });
          await tx.user.update({
            where: { id: createdBy },
            data: { activeRoomId: created.id },
          });
          await tx.roomMember.create({
            data: { roomId: created.id, userId: createdBy, state: 'NOT_READY' },
          });
          return created;
        }, {
          isolationLevel: Prisma.TransactionIsolationLevel.Serializable,
        });
        this.realtimeEvents.emitAll('rooms:update');
        return room;
      } catch (error) {
        if (error instanceof Prisma.PrismaClientKnownRequestError && error.code === 'P2034' && attempt < 2) {
          continue;
        }
        throw error;
      }
    }
    throw new BadRequestException('Création de room impossible');
  }

  async findAll() {
    const now = new Date();
    return this.prisma.room.findMany({
      where: {
        status: 'WAITING',
        OR: [{ lobbyExpiresAt: null }, { lobbyExpiresAt: { gt: now } }],
      },
      orderBy: { createdAt: 'desc' },
      include: { host: { select: { id: true, email: true, displayName: true, avatarUrl: true, status: true } }, members: { select: { userId: true, ready: true, state: true } } },
    });
  }

  async findOne(id: string) {
    const room = await this.prisma.room.findUnique({
      where: { id },
      include: {
        host: { select: { id: true, email: true, displayName: true, avatarUrl: true, status: true } },
        game: { select: { id: true, status: true, currentRoundIndex: true, startedAt: true, finishedAt: true } },
        members: { orderBy: { joinedAt: 'asc' }, include: { user: { select: { id: true, email: true, displayName: true, avatarUrl: true, status: true, virtualBalance: true } } } },
        rounds: { orderBy: { createdAt: 'asc' }, select: { id: true, gameId: true, status: true, opensAt: true, locksAt: true, revealedAt: true, createdAt: true, updatedAt: true } },
      },
    });
    if (!room) throw new NotFoundException(`Salon ${id} introuvable`);
    return room;
  }

  async getState(id: string, requestedRoundId?: string) {
    const room = await this.findOne(id);
    const activeRound = room.rounds.find((r) => ['OPEN', 'LOCKED', 'REVEALED'].includes(r.status));
    const roundId = requestedRoundId || activeRound?.id;
    const round = roundId ? await this.findRoundForRoom(id, roundId) : null;
    const bets = round?.bets ?? [];
    const answeredBets = bets.filter((b: any) => b.answer !== null);
    const totalAnsweredStake = answeredBets.reduce((sum: number, bet: any) => sum + bet.stake, 0);
    const stakeByAnswer = (answer: 'FACT' | 'FAKE') => answeredBets
      .filter((bet: any) => bet.answer === answer)
      .reduce((sum: number, bet: any) => sum + bet.stake, 0);
    const multiplierFor = (answer: 'FACT' | 'FAKE') => totalAnsweredStake > 0
      ? Number(((2 - stakeByAnswer(answer) / totalAnsweredStake) * 1.1).toFixed(2))
      : null;

    return {
      room: { id: room.id, name: room.name, status: room.status, createdBy: room.createdBy, lobbyExpiresAt: room.lobbyExpiresAt, game: room.game },
      members: room.members.map((m) => ({ ...m.user, ready: m.ready, state: m.state, joinedAt: m.joinedAt })),
      round: round ? this.sanitizeRound(round) : null,
      multipliers: { FACT: multiplierFor('FACT'), FAKE: multiplierFor('FAKE') },
      pot: totalAnsweredStake,
      betCount: answeredBets.length,
      serverTime: new Date().toISOString(),
    };
  }

  async getMembers(id: string) {
    const room = await this.findOne(id);
    return room.members.map((m) => ({ ...m.user, ready: m.ready, state: m.state, joinedAt: m.joinedAt }));
  }

  async getHistory(id: string) {
    const room = await this.prisma.room.findUnique({ where: { id }, include: { host: { select: { id: true, email: true, displayName: true, avatarUrl: true, status: true } }, rounds: { orderBy: [{ createdAt: 'desc' }, { id: 'asc' }], include: this.roundHistoryInclude() } } });
    if (!room) throw new NotFoundException(`Salon ${id} introuvable`);
    return { ...room, rounds: room.rounds.map((r) => this.sanitizeRound(r)), messages: await this.chatService.findByRoom(id) };
  }

  async getRoundHistory(id: string) {
    const round = await this.prisma.round.findUnique({ where: { id }, include: this.roundHistoryInclude() });
    if (!round) throw new NotFoundException(`La manche ${id} est introuvable`);
    return this.sanitizeRound(round);
  }

  async join(roomId: string, userId: string) {
    const result = await this.prisma.$transaction(async (tx) => {
      await this.lockUser(tx, userId);

      const locked = await tx.room.updateMany({
        where: { id: roomId },
        data: { updatedAt: new Date() },
      });
      if (locked.count !== 1) throw new NotFoundException('Ce salon n’existe plus');

      const room = await tx.room.findUnique({
        where: { id: roomId },
        select: { id: true, status: true, lobbyExpiresAt: true },
      });
      if (!room) throw new NotFoundException('Ce salon n’existe plus');

      const ban = await tx.roomBan.findUnique({
        where: { roomId_userId: { roomId, userId } },
        select: { id: true },
      });
      if (ban) throw new BadRequestException('Vous avez été expulsé de ce salon');

      const existing = await tx.roomMember.findUnique({
        where: { roomId_userId: { roomId, userId } },
      });
      if (room.status !== 'WAITING' && !existing) {
        throw new BadRequestException('Cette Game a déjà commencé et n’accepte plus de nouveaux joueurs');
      }
      if (room.status === 'WAITING' && !existing) {
        const count = await tx.roomMember.count({ where: { roomId } });
        if (count >= 5) throw new BadRequestException('La room est pleine (5 joueurs maximum)');
      }

      await this.reserveActiveRoom(tx, userId, roomId);

      const expires = room.status === 'WAITING'
        ? new Date(Date.now() + RoomsService.LOBBY_TIMEOUT_MS)
        : room.lobbyExpiresAt;
      const member = await tx.roomMember.upsert({
        where: { roomId_userId: { roomId, userId } },
        update: {
          state: room.status === 'WAITING'
            ? undefined
            : room.status === 'IN_PROGRESS'
              ? 'PLAYING'
              : 'FINISHED',
        },
        create: {
          roomId,
          userId,
          state: room.status === 'WAITING'
            ? 'NOT_READY'
            : room.status === 'IN_PROGRESS'
              ? 'PLAYING'
              : 'FINISHED',
        },
      });
      if (room.status === 'WAITING') {
        await tx.room.update({
          where: { id: roomId },
          data: { lobbyExpiresAt: expires },
        });
      }
      return { member, status: room.status };
    });
    return result.member;
  }

  async leaveLobby(roomId: string, userId: string) {
    const result = await this.prisma.$transaction(async (tx) => {
      await this.lockUser(tx, userId);
      const locked = await tx.room.updateMany({
        where: { id: roomId, status: 'WAITING' },
        data: { updatedAt: new Date() },
      });
      if (locked.count !== 1) return { deleted: false, reset: false };
      const deleted = await tx.roomMember.deleteMany({ where: { roomId, userId } });
      if (!deleted.count) return { deleted: false, reset: false };
      await tx.user.updateMany({ where: { id: userId, activeRoomId: roomId }, data: { activeRoomId: null } });
      await tx.room.updateMany({
        where: { id: roomId, status: 'WAITING' },
        data: { lobbyExpiresAt: new Date(Date.now() + RoomsService.LOBBY_TIMEOUT_MS) },
      });
      return { deleted: true, reset: true };
    });
    if (result.deleted) {
      await this.destroyRoomIfEmpty(roomId);
    }
    return result.deleted;
  }

  async resetLobbyTimer(roomId: string) {
    const expires = new Date(Date.now() + RoomsService.LOBBY_TIMEOUT_MS);
    await this.prisma.$transaction(async (tx) => {
      const locked = await tx.room.updateMany({
        where: { id: roomId, status: 'WAITING' },
        data: { updatedAt: new Date() },
      });
      if (locked.count !== 1) return;
      await tx.room.update({ where: { id: roomId }, data: { lobbyExpiresAt: expires } });
    });
    return expires;
  }


  async setNotReadyOnDisconnect(roomId: string, userId: string) {
    return this.prisma.$transaction(async (tx) => {
      const locked = await tx.room.updateMany({
        where: { id: roomId, status: 'WAITING' },
        data: { updatedAt: new Date() },
      });
      if (locked.count !== 1) return false;
      const member = await tx.roomMember.findUnique({ where: { roomId_userId: { roomId, userId } } });
      if (!member || member.state !== 'READY') return false;
      await tx.roomMember.update({ where: { id: member.id }, data: { ready: false, state: 'NOT_READY' } });
      return true;
    });
  }

  async setReady(roomId: string, userId: string) {
    return this.prisma.$transaction(async (tx) => {
      const locked = await tx.room.updateMany({
        where: { id: roomId, status: 'WAITING' },
        data: { updatedAt: new Date() },
      });
      if (locked.count !== 1) throw new BadRequestException('Le lobby n’est plus disponible');

      const member = await tx.roomMember.findUnique({
        where: { roomId_userId: { roomId, userId } },
      });
      if (!member) throw new BadRequestException('Rejoignez d’abord le lobby');
      if (member.ready) return member;
      return tx.roomMember.update({
        where: { id: member.id },
        data: { ready: true, state: 'READY' },
      });
    });
  }

  async kick(roomId: string, requesterId: string, targetId: string) {
    if (requesterId === targetId) throw new BadRequestException('Vous ne pouvez pas vous expulser vous-même');

    const result = await this.prisma.$transaction(async (tx) => {
      await this.lockUser(tx, targetId);
      const locked = await tx.room.updateMany({
        where: { id: roomId, status: 'WAITING' },
        data: { updatedAt: new Date() },
      });
      if (locked.count !== 1) throw new BadRequestException('Le kick est uniquement disponible dans le lobby');

      const room = await tx.room.findUnique({
        where: { id: roomId },
        select: { status: true, lobbyExpiresAt: true },
      });
      if (!room || room.status !== 'WAITING') throw new BadRequestException('Le kick est uniquement disponible dans le lobby');

      const requester = await tx.roomMember.findUnique({
        where: { roomId_userId: { roomId, userId: requesterId } },
      });
      if (!requester) throw new BadRequestException('Vous ne faites plus partie de ce lobby');

      const target = await tx.roomMember.findUnique({
        where: { roomId_userId: { roomId, userId: targetId } },
      });
      if (!target) {
        const lobbyExpiresAt = new Date(Date.now() + RoomsService.LOBBY_TIMEOUT_MS);
        await tx.room.update({ where: { id: roomId }, data: { lobbyExpiresAt } });
        return { targetId, alreadyKicked: true, lobbyExpiresAt };
      }
      if (target.ready) throw new BadRequestException('Un joueur prêt ne peut pas être expulsé');
      if (Date.now() - target.joinedAt.getTime() < RoomsService.KICK_AFTER_MS) {
        throw new BadRequestException('Le kick est disponible après 45 secondes');
      }

      await tx.roomBan.upsert({
        where: { roomId_userId: { roomId, userId: targetId } },
        update: {},
        create: { roomId, userId: targetId },
      });
      const deleted = await tx.roomMember.deleteMany({
        where: { id: target.id, ready: false },
      });
      if (deleted.count !== 1) throw new BadRequestException('Le joueur n’est plus expulsable');
      await tx.user.updateMany({ where: { id: targetId, activeRoomId: roomId }, data: { activeRoomId: null } });

      const lobbyExpiresAt = new Date(Date.now() + RoomsService.LOBBY_TIMEOUT_MS);
      await tx.room.update({ where: { id: roomId }, data: { lobbyExpiresAt } });
      return { targetId, lobbyExpiresAt };
    });
    return result;
  }

  async closeFinishedRoom(roomId: string) {
    return this.prisma.$transaction(async (tx) => {
      const room = await tx.room.findUnique({
        where: { id: roomId },
        select: { status: true },
      });
      if (!room || room.status !== 'FINISHED') return [];

      const members = await tx.roomMember.findMany({
        where: { roomId },
        select: { userId: true },
      });
      const userIds = members.map((member) => member.userId);

      await tx.user.updateMany({
        where: { activeRoomId: roomId },
        data: { activeRoomId: null },
      });
      await tx.roomMember.deleteMany({ where: { roomId } });
      return userIds;
    });
  }

  async destroyRoomIfEmpty(roomId: string) {
    const destroyed = await this.prisma.$transaction(async (tx) => {
      const reservedUsers = await tx.user.findMany({
        where: { activeRoomId: roomId },
        select: { id: true },
      });
      for (const user of reservedUsers.sort((a, b) => a.id.localeCompare(b.id))) {
        await this.lockUser(tx, user.id);
      }

      const locked = await tx.room.updateMany({
        where: { id: roomId },
        data: { updatedAt: new Date() },
      });
      if (locked.count !== 1) return false;

      const room = await tx.room.findUnique({
        where: { id: roomId },
        select: { id: true, status: true },
      });
      if (!room) return false;

      const memberCount = await tx.roomMember.count({ where: { roomId } });
      if (memberCount !== 0) return false;

      if (room.status === 'FINISHED') {
        await tx.user.updateMany({ where: { activeRoomId: roomId }, data: { activeRoomId: null } });
        await tx.message.deleteMany({ where: { roomId } });
        await tx.roomBan.deleteMany({ where: { roomId } });
        return false;
      }

      await tx.user.updateMany({ where: { activeRoomId: roomId }, data: { activeRoomId: null } });

      await tx.message.deleteMany({ where: { roomId } });
      await tx.clipVote.deleteMany({ where: { round: { roomId } } });
      await tx.bet.deleteMany({ where: { round: { roomId } } });
      await tx.roundClaim.deleteMany({ where: { round: { roomId } } });
      await tx.round.deleteMany({ where: { roomId } });
      await tx.game.deleteMany({ where: { roomId } });
      const deleted = await tx.room.deleteMany({ where: { id: roomId } });
      return deleted.count === 1;
    });

    if (destroyed) {
      this.realtimeEvents.emitToRoom(roomId, 'room:destroyed', { roomId });
      await this.realtimeEvents.ejectRoom(roomId);
      this.realtimeEvents.emitAll('rooms:update');
    }
    return destroyed;
  }

  async deleteExpiredRoom(roomId: string) {
    return this.prisma.$transaction(async (tx) => {
      const reservedUsers = await tx.user.findMany({
        where: { activeRoomId: roomId },
        select: { id: true },
      });
      for (const user of reservedUsers.sort((a, b) => a.id.localeCompare(b.id))) {
        await this.lockUser(tx, user.id);
      }

      const locked = await tx.room.updateMany({
        where: { id: roomId, status: 'WAITING', lobbyExpiresAt: { lte: new Date() } },
        data: { updatedAt: new Date() },
      });
      if (locked.count !== 1) return false;
      const members = await tx.roomMember.findMany({
        where: { roomId },
        select: { ready: true },
      });
      if (members.length >= 2 && members.every((member) => member.ready)) return false;
      await tx.user.updateMany({ where: { activeRoomId: roomId }, data: { activeRoomId: null } });
      const result = await tx.room.deleteMany({
        where: { id: roomId, status: 'WAITING', lobbyExpiresAt: { lte: new Date() } },
      });
      return result.count === 1;
    });
  }

  private async lockUser(tx: Prisma.TransactionClient, userId: string) {
    const locked = await tx.user.updateMany({
      where: { id: userId },
      data: { updatedAt: new Date() },
    });
    if (locked.count !== 1) throw new NotFoundException('Utilisateur introuvable');
  }

  private async reserveActiveRoom(tx: Prisma.TransactionClient, userId: string, targetRoomId: string | null) {
    const user = await tx.user.findUnique({
      where: { id: userId },
      select: { activeRoomId: true, activeRoom: { select: { id: true, status: true } } },
    });
    if (!user) throw new NotFoundException('Utilisateur introuvable');

    if (user.activeRoomId && user.activeRoomId !== targetRoomId) {
      if (user.activeRoom && ['WAITING', 'IN_PROGRESS'].includes(user.activeRoom.status)) {
        throw new ConflictException('USER_ALREADY_IN_ROOM');
      }
      await tx.user.update({ where: { id: userId }, data: { activeRoomId: null } });
    }

    if (targetRoomId) {
      await tx.user.update({
        where: { id: userId },
        data: { activeRoomId: targetRoomId },
      });
    }
  }

  async assertCanPlay(_userId: string) { return true; }

  private async findRoundForRoom(roomId: string, roundId: string) {
    return this.prisma.round.findFirst({ where: { id: roundId, roomId }, include: this.roundHistoryInclude() });
  }

  private roundHistoryInclude() {
    return {
      room: { select: { id: true, name: true, status: true, createdBy: true } },
      game: { select: { id: true, status: true, currentRoundIndex: true } },
      roundClaims: { orderBy: { orderIndex: 'asc' as const }, include: { claim: true } },
      bets: { orderBy: [{ createdAt: 'asc' as const }, { id: 'asc' as const }], include: { user: { select: { id: true, email: true, displayName: true, avatarUrl: true, status: true, virtualBalance: true } } } },
      clipVotes: true,
    };
  }

  private sanitizeRound(round: any) {
    const revealed = ['REVEALED', 'FINISHED'].includes(String(round.status));
    const { bets, clipVotes: _clipVotes, ...safeRound } = round;
    return {
      ...safeRound,
      ...(revealed ? {
        bets,
        transitionEndsAt: round.revealedAt
          ? new Date(round.revealedAt.getTime() + RESULT_DISPLAY_MS)
          : null,
      } : {}),
      roundClaims: (round.roundClaims ?? []).map((rc: any) => {
        const { truthLabel, ...safeClaim } = rc.claim;
        return { id: rc.id, roundId: rc.roundId, claimId: rc.claimId, orderIndex: rc.orderIndex, claim: revealed ? rc.claim : safeClaim };
      }),
    };
  }
}
