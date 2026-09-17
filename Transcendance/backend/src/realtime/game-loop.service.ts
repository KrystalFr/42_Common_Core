import { Injectable, Logger, OnModuleDestroy, OnModuleInit } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service.js';
import { RealtimeGateway } from './realtime.gateway.js';
import { GameService } from './game.service.js';
import { RoomsService } from '../rooms/rooms.service.js';
import { RealtimeInvalidationService } from './realtime-invalidation.service.js';

@Injectable()
export class GameLoopService implements OnModuleInit, OnModuleDestroy {
  private readonly logger = new Logger(GameLoopService.name);
  private timer?: NodeJS.Timeout;
  private running = false;
  private startupRecovered = false;

  constructor(
    private readonly prisma: PrismaService,
    private readonly games: GameService,
    private readonly gateway: RealtimeGateway,
    private readonly rooms: RoomsService,
    private readonly invalidations: RealtimeInvalidationService,
  ) {}

  onModuleInit() {
    void this.tick();
    this.timer = setInterval(() => void this.tick(), 250);
  }

  onModuleDestroy() {
    if (this.timer) clearInterval(this.timer);
  }

  private async tick() {
    if (this.running) return;
    this.running = true;
    try {
      if (!this.startupRecovered) {
        const interruptedRooms = await this.games.cancelInterruptedGames();
        for (const roomId of interruptedRooms) {
          this.gateway.server?.to(`room:${roomId}`).emit('game:cancelled', { roomId, reason: 'backend_restart' });
          await this.gateway.publishRoomState(roomId, 'game:cancelled');
          await this.gateway.ejectRoom(roomId);
        }
        this.startupRecovered = true;
      }
      const startedRooms = await this.games.startReadyGames();
      for (const roomId of startedRooms) {
        await this.gateway.publishRoomState(roomId, 'game:started');
        this.gateway.broadcastRoomListUpdate();
      }
      const deletedRooms = await this.games.expireWaitingRooms();
      for (const roomId of deletedRooms) {
        this.gateway.server?.to(`room:${roomId}`).emit('room:expired', { roomId });
        await this.gateway.ejectRoom(roomId);
        this.gateway.broadcastRoomListUpdate();
      }

      const finishedRooms = await this.prisma.game.findMany({
        where: {
          status: 'FINISHED',
          finishedAt: { lte: new Date(Date.now() - 60_000) },
        },
        select: { roomId: true },
      });
      for (const game of finishedRooms) {
        try {
          await this.gateway.closeFinishedRoom(game.roomId);
        } catch (error) {
          this.logger.error(`Finished room cleanup failed for ${game.roomId}: ${error instanceof Error ? error.message : String(error)}`);
        }
      }

      const expiredRounds = await this.prisma.round.findMany({
        where: { status: 'OPEN', game: { status: 'IN_PROGRESS' } },
        select: { id: true, roomId: true, gameId: true },
      });
      for (const round of expiredRounds) {
        try {
          const result = await this.games.resolveRound(round.id);
          if (!result) continue;

          if (result.userIds.length > 0) {
            this.invalidations.afterRoundResolution({ userIds: result.userIds });
          }

          const { kicked, cancelledUserId } = await this.games.penalizeMissingPlayers(round.id);
          for (const userId of kicked) {
            this.gateway.server?.to(`room:${round.roomId}`).emit('room:kicked', { userId, reason: 'absence' });
            await this.gateway.broadcastBalanceUpdate(userId);
            await this.gateway.ejectUserFromRoom(round.roomId, userId);
          }
          if (cancelledUserId) {
            this.gateway.server?.to(`room:${round.roomId}`).emit('game:cancelled', {
              roomId: round.roomId,
              reason: 'single_player',
            });
            await this.gateway.broadcastBalanceUpdate(cancelledUserId);
          }

          const remainingUsers = await this.prisma.roomMember.findMany({
            where: { roomId: round.roomId },
            select: { userId: true },
          });
          for (const member of remainingUsers) {
            await this.gateway.broadcastBalanceUpdate(member.userId);
          }

          const remainingMembers = remainingUsers.length;
          if (remainingMembers === 0) {
            await this.rooms.destroyRoomIfEmpty(round.roomId);
            continue;
          }
          await this.gateway.publishRoundState(round.roomId, 'round:revealed', round.id);
          this.gateway.publishRoundVerdict(round.roomId, round.id, result.truth, result.total, result.good);
        } catch (error) {
          this.logger.error(`Round ${round.id} resolution failed: ${error instanceof Error ? error.message : String(error)}`);
        }
      }

      const revealed = await this.prisma.round.findMany({
        where: { status: 'REVEALED', game: { status: 'IN_PROGRESS' } },
        select: { id: true, roomId: true, gameId: true, revealedAt: true },
      });
      for (const round of revealed) {
        if (!round.gameId || !round.revealedAt) continue;
        const advanced = await this.games.advanceAfterReveal(round.gameId);
        if (!advanced) continue;
        if (advanced.finished) {
          const finishedGame = await this.prisma.game.findUnique({
            where: { id: round.gameId },
            select: { finishedAt: true },
          });
          await this.gateway.publishRoomState(round.roomId, 'game:finished');
          this.gateway.scheduleFinishedRoomCleanup(round.roomId, finishedGame?.finishedAt);
        } else {
          await this.gateway.publishRoomState(round.roomId, 'round:started', advanced.roundId);
        }
      }
    } catch (error) {
      this.logger.error(`Game loop failed: ${error instanceof Error ? error.message : String(error)}`);
    } finally {
      this.running = false;
    }
  }
}
