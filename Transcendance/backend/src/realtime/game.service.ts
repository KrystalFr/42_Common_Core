import { BadRequestException, Injectable, Logger, NotFoundException } from '@nestjs/common';
import { randomInt } from 'node:crypto';
import { BetAnswer, Prisma } from '@prisma/client';
import { PrismaService } from '../prisma/prisma.service.js';
import { LedgerService } from '../ledger/ledger.service.js';
import { VideoDurationService } from '../rounds/video-duration.service.js';
import { calculatePayout, RESULT_DISPLAY_MS } from './game-rules.js';

const GAME_ROUNDS = 5;

type AdvanceAfterRevealResult =
  | { finished: true }
  | { finished: false; roundId: string };

@Injectable()
export class GameService {
  private readonly logger = new Logger(GameService.name);
  private readonly startRetryAt = new Map<string, number>();
  private static readonly START_RETRY_DELAY_MS = 5_000;

  constructor(
    private readonly prisma: PrismaService,
    private readonly ledger: LedgerService,
    private readonly videoDuration: VideoDurationService,
  ) {}

  async cancelInterruptedGames() {
    const games = await this.prisma.game.findMany({
      where: { status: { in: ['STARTING', 'IN_PROGRESS'] } },
      select: { id: true, roomId: true },
    });
    const cancelled: string[] = [];
    for (const game of games) {
      const done = await this.prisma.$transaction(async (tx) => {
        const members = await tx.roomMember.findMany({ where: { roomId: game.roomId }, select: { userId: true } });
        await this.lockUsers(tx, members.map((m) => m.userId));
        const lockedRoom = await tx.room.updateMany({
          where: { id: game.roomId, status: 'IN_PROGRESS' },
          data: { updatedAt: new Date() },
        });
        if (lockedRoom.count !== 1) return false;
        const current = await tx.game.findUnique({ where: { id: game.id }, select: { status: true } });
        if (!current || !['STARTING', 'IN_PROGRESS'].includes(current.status)) return false;
        const bets = await tx.bet.findMany({
          where: { round: { gameId: game.id }, debitedAt: { not: null }, result: null },
          select: { id: true, userId: true, stake: true },
        });
        for (const bet of bets) {
          await this.ledger.refundBet(tx, bet.userId, bet.stake, `refund:${bet.id}`);
          await tx.bet.update({ where: { id: bet.id }, data: { result: 'CANCELLED', payout: 0 } });
        }
        await tx.round.updateMany({ where: { gameId: game.id, status: { in: ['COUNTDOWN', 'OPEN', 'LOCKED', 'REVEALED'] } }, data: { status: 'FINISHED' } });
        await tx.game.updateMany({ where: { id: game.id, status: { in: ['STARTING', 'IN_PROGRESS'] } }, data: { status: 'FINISHED', finishedAt: new Date() } });
        await tx.room.updateMany({ where: { id: game.roomId, status: 'IN_PROGRESS' }, data: { status: 'FINISHED' } });
        await tx.roomMember.updateMany({ where: { roomId: game.roomId }, data: { state: 'FINISHED' } });
        await tx.user.updateMany({ where: { activeRoomId: game.roomId }, data: { activeRoomId: null } });
        return true;
      });
      if (done) cancelled.push(game.roomId);
    }
    return cancelled;
  }

  async startReadyGames() {
    const rooms = await this.prisma.room.findMany({
      where: { status: 'WAITING', OR: [{ lobbyExpiresAt: null }, { lobbyExpiresAt: { gt: new Date() } }] },
      select: { id: true },
    });
    const started: string[] = [];
    for (const room of rooms) {
      const retryAt = this.startRetryAt.get(room.id) ?? 0;
      if (retryAt > Date.now()) continue;
      try {
        const ok = await this.startIfReady(room.id);
        this.startRetryAt.delete(room.id);
        if (ok) started.push(room.id);
      } catch (error) {
        this.startRetryAt.set(room.id, Date.now() + GameService.START_RETRY_DELAY_MS);
        this.logger.warn(`GAME_START_BLOCKED room=${room.id}: ${error instanceof Error ? error.message : String(error)}`);
      }
    }
    return started;
  }

  async expireWaitingRooms() {
    const expired = await this.prisma.room.findMany({
      where: { status: 'WAITING', lobbyExpiresAt: { lte: new Date() } },
      select: { id: true },
    });
    const deletedRooms: string[] = [];
    for (const room of expired) {
      const deleted = await this.prisma.$transaction(async (tx) => {
        const reservedUsers = await tx.user.findMany({
          where: { activeRoomId: room.id },
          select: { id: true },
        });
        for (const user of reservedUsers.sort((a, b) => a.id.localeCompare(b.id))) {
          await this.lockUser(tx, user.id);
        }

        const locked = await tx.room.updateMany({
          where: { id: room.id, status: 'WAITING', lobbyExpiresAt: { lte: new Date() } },
          data: { updatedAt: new Date() },
        });
        if (locked.count !== 1) return false;

        const members = await tx.roomMember.findMany({
          where: { roomId: room.id },
          select: { ready: true },
        });
        if (members.length >= 2 && members.every((member) => member.ready)) return false;

        await tx.user.updateMany({
          where: { activeRoomId: room.id },
          data: { activeRoomId: null },
        });

        const result = await tx.room.deleteMany({
          where: { id: room.id, status: 'WAITING', lobbyExpiresAt: { lte: new Date() } },
        });
        return result.count === 1;
      });
      if (deleted) deletedRooms.push(room.id);
    }
    return deletedRooms;
  }

  async startIfReady(roomId: string) {
    const reservation = await this.prisma.$transaction(async (tx) => {
      const lockedRoom = await tx.room.updateMany({
        where: { id: roomId, status: 'WAITING' },
        data: { updatedAt: new Date() },
      });
      if (lockedRoom.count !== 1) return null;
      const members = await tx.roomMember.findMany({
        where: { roomId },
        orderBy: { joinedAt: 'asc' },
        select: { userId: true, ready: true },
      });
      if (members.length < 2 || members.some((m) => !m.ready)) return null;
      const existing = await tx.game.findUnique({ where: { roomId }, select: { id: true, status: true } });
      if (existing) return existing.id;
      const started = await tx.room.updateMany({
        where: { id: roomId, status: 'WAITING' },
        data: { status: 'IN_PROGRESS', lobbyExpiresAt: null },
      });
      if (started.count !== 1) return null;
      const game = await tx.game.create({ data: { roomId, status: 'STARTING', currentRoundIndex: 0 } });
      await tx.roomMember.updateMany({ where: { roomId }, data: { state: 'PLAYING' } });
      return game.id;
    });
    if (!reservation) return null;

    const candidates = await this.pickFiveClaims(roomId);
    const resolved: Array<{ claimId: string; duration: number }> = [];
    const failures: Array<{ claimId: string; reason: string }> = [];
    for (const candidate of candidates) {
      try {
        resolved.push({ claimId: candidate.id, duration: await this.videoDuration.ensureDuration(candidate.id) });
      } catch (error) {
        failures.push({ claimId: candidate.id, reason: error instanceof Error ? error.message : String(error) });
      }
      if (resolved.length === GAME_ROUNDS) break;
    }
    if (resolved.length < GAME_ROUNDS) {
      await this.cancelStartingGame(roomId, reservation);
      const detail = failures.map((failure) => `${failure.claimId}: ${failure.reason}`).join('; ');
      throw new BadRequestException(`GAME_START_BLOCKED: ${resolved.length}/${GAME_ROUNDS} vidéos résolues${detail ? ` — ${detail}` : ''}`);
    }

    const completed = await this.prisma.$transaction(async (tx) => {
      const lockedRoom = await tx.room.updateMany({ where: { id: roomId, status: 'IN_PROGRESS' }, data: { updatedAt: new Date() } });
      if (lockedRoom.count !== 1) return null;
      const lockedMembers = await tx.roomMember.findMany({ where: { roomId }, orderBy: { joinedAt: 'asc' }, select: { userId: true, ready: true, state: true } });
      if (lockedMembers.length < 2 || lockedMembers.some((m) => !m.ready || m.state !== 'PLAYING')) return null;
      const game = await tx.game.findUnique({ where: { id: reservation }, select: { id: true, status: true } });
      if (!game || game.status !== 'STARTING') return null;
      for (let index = 0; index < resolved.length; index += 1) {
        const now = new Date();
        const duration = resolved[index].duration + 30;
        await tx.round.create({
          data: {
            roomId,
            gameId: game.id,
            status: index === 0 ? 'OPEN' : 'COUNTDOWN',
            opensAt: index === 0 ? now : new Date('2099-01-01T00:00:00.000Z'),
            locksAt: index === 0 ? new Date(now.getTime() + duration * 1000) : new Date('2099-01-01T00:00:00.000Z'),
            roundClaims: { create: { claimId: resolved[index].claimId, orderIndex: 0 } },
          },
        });
      }
      await tx.game.updateMany({ where: { id: game.id, status: 'STARTING' }, data: { status: 'IN_PROGRESS', startedAt: new Date() } });
      return game.id;
    });
    if (!completed) {
      await this.cancelStartingGame(roomId, reservation);
      return null;
    }
    return completed;
  }

  private async cancelStartingGame(roomId: string, gameId: string) {
    await this.prisma.$transaction(async (tx) => {
      const locked = await tx.room.updateMany({ where: { id: roomId, status: 'IN_PROGRESS' }, data: { updatedAt: new Date() } });
      if (locked.count !== 1) return;
      const game = await tx.game.findUnique({ where: { id: gameId }, select: { status: true } });
      if (!game || game.status !== 'STARTING') return;
      await tx.game.deleteMany({ where: { id: gameId, status: 'STARTING' } });
      await tx.room.updateMany({ where: { id: roomId, status: 'IN_PROGRESS' }, data: { status: 'WAITING', lobbyExpiresAt: new Date(Date.now() + 90_000) } });
      await tx.roomMember.updateMany({ where: { roomId }, data: { state: 'READY' } });
    });
  }

  async pickFiveClaims(roomId: string) {
    void roomId;
    const candidates = await this.prisma.claim.findMany({
      where: { mediaRef: { not: null } },
      select: { id: true, sourceKey: true },
      orderBy: [{ sourceKey: 'asc' }, { id: 'asc' }],
      take: 50,
    });

    for (let index = candidates.length - 1; index > 0; index -= 1) {
      const swapIndex = randomInt(index + 1);
      [candidates[index], candidates[swapIndex]] = [candidates[swapIndex], candidates[index]];
    }

    return candidates;
  }

  async confirmBet(roundId: string, userId: string, stake: number) {
    if (!Number.isSafeInteger(stake) || stake <= 0) throw new BadRequestException('Mise invalide');
    const round = await this.prisma.round.findUnique({ where: { id: roundId }, select: { id: true, roomId: true, status: true, locksAt: true, gameId: true } });
    if (!round || !round.gameId) throw new NotFoundException('Manche introuvable');
    if (round.status !== 'OPEN' || round.locksAt.getTime() <= Date.now()) throw new BadRequestException('La manche est fermée');
    const member = await this.prisma.roomMember.findUnique({ where: { roomId_userId: { roomId: round.roomId, userId } }, select: { state: true } });
    if (!member || !['PLAYING', 'READY'].includes(member.state)) throw new BadRequestException('Vous ne participez plus à cette Game');
    const user = await this.prisma.user.findUnique({ where: { id: userId }, select: { virtualBalance: true } });
    if (!user) throw new NotFoundException('Utilisateur introuvable');
    const minimum = user.virtualBalance >= 10 ? 10 : user.virtualBalance;
    if (user.virtualBalance <= 0) throw new BadRequestException('Solde nul : récupérez vos 200 crédits avant de miser');
    if (stake < minimum || stake > user.virtualBalance) throw new BadRequestException(`Mise autorisée : ${minimum} à ${user.virtualBalance} crédits`);

    const existing = await this.prisma.bet.findUnique({ where: { userId_roundId: { userId, roundId } } });
    if (existing?.answer) return { ...existing, alreadyAnswered: true };
    return this.prisma.bet.upsert({
      where: { userId_roundId: { userId, roundId } },
      update: { stake, amountConfirmedAt: new Date() },
      create: { userId, roundId, stake, amountConfirmedAt: new Date() },
    });
  }

  async answer(roundId: string, userId: string, answer: BetAnswer) {
    const now = new Date();
    return this.prisma.$transaction(async (tx) => {
      const target = await tx.round.findUnique({ where: { id: roundId }, select: { roomId: true } });
      if (!target) throw new NotFoundException('Manche introuvable');
      await this.lockUser(tx, userId);
      const roomLock = await tx.room.updateMany({
        where: { id: target.roomId, status: 'IN_PROGRESS' },
        data: { updatedAt: new Date() },
      });
      if (roomLock.count !== 1) throw new BadRequestException('La partie n’est plus active');

      const round = await tx.round.findUnique({
        where: { id: roundId },
        include: { roundClaims: { include: { claim: { select: { truthLabel: true } } } } },
      });
      if (!round || round.status !== 'OPEN' || round.locksAt.getTime() <= now.getTime()) throw new BadRequestException('La manche est expirée');
      const member = await tx.roomMember.findUnique({ where: { roomId_userId: { roomId: round.roomId, userId } } });
      if (!member || member.state !== 'PLAYING') throw new BadRequestException('Vous ne participez plus à cette Game');
      const bet = await tx.bet.findUnique({ where: { userId_roundId: { userId, roundId } } });
      if (!bet) throw new BadRequestException('Confirmez d’abord le montant de votre mise');
      if (bet.answer) return bet;
      const reserved = await tx.bet.updateMany({ where: { id: bet.id, answer: null }, data: { answer, answeredAt: now, debitedAt: now } });
      if (reserved.count !== 1) return tx.bet.findUniqueOrThrow({ where: { id: bet.id } });
      const user = await tx.user.findUnique({ where: { id: userId }, select: { virtualBalance: true } });
      if (!user || bet.stake <= 0 || bet.stake > user.virtualBalance) throw new BadRequestException('Solde insuffisant pour cette mise');
      await this.ledger.debitForBet(tx, userId, bet.stake, `bet:${bet.id}`);
      const claimId = round.roundClaims[0]?.claimId;
      if (claimId) {
        await tx.clipVote.upsert({
          where: { userId_roundId_claimId: { userId, roundId, claimId } },
          update: { isFake: answer === 'FAKE' },
          create: { userId, roundId, claimId, isFake: answer === 'FAKE' },
        });
      }
      return tx.bet.findUniqueOrThrow({ where: { id: bet.id } });
    });
  }

  async resolveRound(roundId: string) {
    return this.prisma.$transaction(async (tx) => {
      const target = await tx.round.findUnique({ where: { id: roundId }, select: { roomId: true } });
      if (!target) return null;
      const roundUsers = await tx.roomMember.findMany({
        where: { roomId: target.roomId, state: 'PLAYING' },
        select: { userId: true },
      });
      await this.lockUsers(tx, roundUsers.map((member) => member.userId));
      const roomLock = await tx.room.updateMany({
        where: { id: target.roomId, status: 'IN_PROGRESS' },
        data: { updatedAt: new Date() },
      });
      if (roomLock.count !== 1) return null;

      const round = await tx.round.findUnique({
        where: { id: roundId },
        include: {
          game: true,
          roundClaims: { include: { claim: { select: { truthLabel: true } } } },
          bets: true,
        },
      });
      if (!round || !round.game || round.game.status !== 'IN_PROGRESS' || round.status !== 'OPEN') return null;

      const expectedPlayers = await tx.roomMember.findMany({
        where: { roomId: round.roomId, state: 'PLAYING' },
        select: { userId: true },
      });
      const answeredPlayerIds = new Set(
        round.bets.filter((bet) => bet.answer !== null).map((bet) => bet.userId),
      );
      const allAnswered = expectedPlayers.length > 0 && expectedPlayers.every((player) => answeredPlayerIds.has(player.userId));
      const now = new Date();
      const canResolve = round.locksAt.getTime() <= now.getTime() || allAnswered;
      if (!canResolve) return null;

      const locked = await tx.round.updateMany({
        where: { id: roundId, status: 'OPEN' },
        data: { status: 'LOCKED' },
      });
      if (locked.count !== 1) return null;

      const truth = round.roundClaims[0]?.claim.truthLabel === 'TRUE' ? 'FACT' : 'FAKE';
      const validBets = round.bets.filter((b) => b.answer !== null);
      const total = validBets.reduce((sum, b) => sum + b.stake, 0);
      const good = validBets.filter((b) => b.answer === truth).reduce((sum, b) => sum + b.stake, 0);
      const factor = total > 0 && good > 0 ? 2 - good / total : 0;
      const payouts = new Map<string, number>();
      for (const bet of validBets) {
        const win = bet.answer === truth;
        const payout = calculatePayout(total, good, bet.stake, win);
        payouts.set(bet.id, payout);
        await tx.bet.update({ where: { id: bet.id }, data: { result: win ? 'WIN' : 'LOSS', payout } });
        if (payout > 0) await this.ledger.creditPayout(tx, bet.userId, payout, `payout:${bet.id}`);
      }

      const winningBets = validBets.filter((bet) => bet.answer === truth);
      const winningUserIds = [...new Set(winningBets.map((bet) => bet.userId))];
      if (winningUserIds.length > 0) {
        const humanWinners = await tx.user.findMany({
          where: { id: { in: winningUserIds }, role: { not: 'BOT' } },
          select: { id: true },
        });
        const humanWinnerIds = new Set(humanWinners.map((user) => user.id));
        for (const userId of humanWinnerIds) {
          const userBets = validBets.filter((bet) => bet.userId === userId);
          const netGain = userBets.reduce((sum, bet) => sum + (payouts.get(bet.id) ?? 0) - bet.stake, 0);
          const totalWins = await tx.bet.count({ where: { userId, result: 'WIN' } });
          const gameWins = await tx.bet.count({ where: { userId, round: { gameId: round.gameId }, result: 'WIN' } });
          const achievements = [
            'FIRST_WIN',
            ...(netGain >= 100 ? ['CREDITS_100'] : []),
            ...(userBets.some((bet) => bet.stake === 250) ? ['BET_250_WIN'] : []),
            ...(gameWins >= GAME_ROUNDS ? ['PERFECT_GAME'] : []),
            ...(totalWins >= 10 ? ['TEN_WINS'] : []),
          ];
          await tx.achievementUnlock.createMany({
            data: achievements.map((achievementKey) => ({ userId, achievementKey })),
            skipDuplicates: true,
          });
        }
      }

      for (const userId of new Set(validBets.map((bet) => bet.userId))) {
        await this.ledger.markRecoveryEligible(tx, userId);
      }

      await tx.round.update({ where: { id: roundId }, data: { status: 'REVEALED', revealedAt: new Date() } });
      const userIds = [...new Set(validBets.map((bet) => bet.userId))];
      return { roundId, total, good, factor, truth, userIds };
    });
  }

  async penalizeMissingPlayers(roundId: string) {
    const round = await this.prisma.round.findUnique({ where: { id: roundId }, select: { roomId: true, status: true } });
    if (!round || !['LOCKED', 'REVEALED'].includes(round.status)) return { kicked: [], cancelledUserId: undefined as string | undefined };

    const members = await this.prisma.roomMember.findMany({
      where: { roomId: round.roomId, state: 'PLAYING' },
      select: { id: true, userId: true },
    });
    const bets = await this.prisma.bet.findMany({ where: { roundId }, select: { userId: true, answer: true } });
    const answered = new Set(bets.filter((b) => b.answer !== null).map((b) => b.userId));
    const kicked: string[] = [];
    let cancelledUserId: string | undefined;

    for (const member of members) {
      if (answered.has(member.userId)) continue;

      const result = await this.prisma.$transaction(async (tx) => {
        await this.lockUser(tx, member.userId);
        const roomLock = await tx.room.updateMany({
          where: { id: round.roomId, status: 'IN_PROGRESS' },
          data: { updatedAt: new Date() },
        });
        if (roomLock.count !== 1) return { removed: false, cancelledUserId: undefined as string | undefined };

        const stillMember = await tx.roomMember.findUnique({ where: { id: member.id } });
        if (!stillMember || stillMember.state !== 'PLAYING') return { removed: false, cancelledUserId: undefined as string | undefined };

        await this.ledger.debitPenalty(tx, member.userId, `penalty:absence:${roundId}:${member.userId}`);
        const deleted = await tx.roomMember.deleteMany({ where: { id: member.id } });
        if (deleted.count !== 1) return { removed: false, cancelledUserId: undefined as string | undefined };
        await tx.user.updateMany({ where: { id: member.userId, activeRoomId: round.roomId }, data: { activeRoomId: null } });

        return { removed: true, cancelledUserId: undefined as string | undefined };
      });

      if (result.removed) kicked.push(member.userId);
    }

    if (kicked.length > 0) {
      cancelledUserId = await this.prisma.$transaction(async (tx) => this.cancelSinglePlayerGameTx(tx, round.roomId));
    }
    return { kicked, cancelledUserId };
  }

  async leaveGame(roomId: string, userId: string, referencePrefix = 'leave') {
    return this.prisma.$transaction(async (tx) => {
      const membersBeforeLeave = await tx.roomMember.findMany({
        where: { roomId },
        select: { userId: true },
      });
      await this.lockUsers(tx, [userId, ...membersBeforeLeave.map((member) => member.userId)]);
      const locked = await tx.room.updateMany({
        where: { id: roomId, status: 'IN_PROGRESS' },
        data: { updatedAt: new Date() },
      });
      if (locked.count !== 1) {
        const member = await tx.roomMember.findUnique({ where: { roomId_userId: { roomId, userId } } });
        if (!member) return { removed: false, cancelledUserId: undefined as string | undefined };
        await tx.roomMember.delete({ where: { id: member.id } });
        await tx.user.updateMany({ where: { id: userId, activeRoomId: roomId }, data: { activeRoomId: null } });
        return { removed: true, cancelledUserId: undefined as string | undefined };
      }

      const member = await tx.roomMember.findUnique({ where: { roomId_userId: { roomId, userId } } });
      if (!member) return { removed: false, cancelledUserId: undefined as string | undefined };

      const game = await tx.game.findUnique({ where: { roomId }, select: { id: true, status: true } });
      if (game?.status === 'STARTING') {
        await tx.roomMember.delete({ where: { id: member.id } });
        await tx.user.updateMany({ where: { id: userId, activeRoomId: roomId }, data: { activeRoomId: null } });
        const remaining = await tx.roomMember.count({ where: { roomId, state: 'PLAYING' } });
        if (remaining < 2) {
          await tx.game.deleteMany({ where: { id: game.id, status: 'STARTING' } });
          await tx.room.updateMany({ where: { id: roomId, status: 'IN_PROGRESS' }, data: { status: 'WAITING', lobbyExpiresAt: new Date(Date.now() + 90_000) } });
          await tx.roomMember.updateMany({ where: { roomId, state: 'PLAYING' }, data: { state: 'READY' } });
        }
        return { removed: true, cancelledUserId: undefined as string | undefined };
      }

      await this.ledger.debitPenalty(tx, userId, `penalty:${referencePrefix}:${roomId}:${userId}`);
      await tx.roomMember.delete({ where: { id: member.id } });
      await tx.user.updateMany({ where: { id: userId, activeRoomId: roomId }, data: { activeRoomId: null } });
      const cancelledUserId = await this.cancelSinglePlayerGameTx(tx, roomId);
      return { removed: true, cancelledUserId };
    });
  }

  async cancelGameIfSinglePlayer(roomId: string) {
    return this.prisma.$transaction(async (tx) => {
      const candidate = await tx.roomMember.findMany({
        where: { roomId, state: 'PLAYING' },
        select: { userId: true },
      });
      if (candidate.length !== 1) return undefined;
      await this.lockUser(tx, candidate[0].userId);

      const locked = await tx.room.updateMany({
        where: { id: roomId, status: 'IN_PROGRESS' },
        data: { updatedAt: new Date() },
      });
      if (locked.count !== 1) return false;
      return (await this.cancelSinglePlayerGameTx(tx, roomId)) ?? false;
    });
  }

  private async cancelSinglePlayerGameTx(tx: Prisma.TransactionClient, roomId: string): Promise<string | undefined> {
    const game = await tx.game.findUnique({
      where: { roomId },
      select: { id: true, status: true },
    });
    if (!game || game.status !== 'IN_PROGRESS') return undefined;

    const remaining = await tx.roomMember.findMany({
      where: { roomId, state: 'PLAYING' },
      select: { id: true, userId: true },
    });
    if (remaining.length === 0) {
      const finished = await tx.game.updateMany({ where: { id: game.id, status: 'IN_PROGRESS' }, data: { status: 'FINISHED', finishedAt: new Date() } });
      if (finished.count !== 1) return undefined;
      await tx.round.updateMany({ where: { gameId: game.id, status: { in: ['COUNTDOWN', 'OPEN', 'LOCKED', 'REVEALED'] } }, data: { status: 'FINISHED' } });
      await tx.room.updateMany({ where: { id: roomId, status: 'IN_PROGRESS' }, data: { status: 'FINISHED' } });
      return undefined;
    }
    if (remaining.length !== 1) return undefined;

    const player = remaining[0];
    const bets = await tx.bet.findMany({
      where: {
        userId: player.userId,
        round: { gameId: game.id },
        debitedAt: { not: null },
        result: null,
      },
      select: { id: true, stake: true, debitedAt: true, result: true, payout: true },
    });

    for (const bet of bets) {
      if (bet.debitedAt && bet.stake > 0 && bet.result === null && bet.payout === null) {
        await this.ledger.refundBet(tx, player.userId, bet.stake, `refund:${bet.id}`);
      }
    }

    const finished = await tx.game.updateMany({
      where: { id: game.id, status: 'IN_PROGRESS' },
      data: { status: 'FINISHED', finishedAt: new Date() },
    });
    if (finished.count !== 1) return undefined;

    await tx.round.updateMany({
      where: {
        gameId: game.id,
        status: { in: ['COUNTDOWN', 'OPEN', 'LOCKED', 'REVEALED'] },
      },
      data: { status: 'FINISHED' },
    });
    await tx.room.updateMany({ where: { id: roomId, status: 'IN_PROGRESS' }, data: { status: 'FINISHED' } });
    await tx.user.updateMany({ where: { id: player.userId, activeRoomId: roomId }, data: { activeRoomId: null } });
    await tx.roomMember.deleteMany({ where: { id: player.id } });
    return player.userId;
  }

  async claimRecovery(userId: string) {
    return this.ledger.reclamerRattrapage(userId);
  }

  async cancelRoundForUnavailableVideo(roundId: string, userId: string) {
    return this.prisma.$transaction(async (tx) => {
      const round = await tx.round.findUnique({ where: { id: roundId }, select: { id: true, roomId: true, gameId: true, status: true } });
      if (!round || !round.gameId || round.status !== 'OPEN') throw new BadRequestException('La manche n’est plus ouverte');
      const member = await tx.roomMember.findUnique({ where: { roomId_userId: { roomId: round.roomId, userId } }, select: { state: true } });
      if (!member || member.state !== 'PLAYING') throw new BadRequestException('Vous ne participez plus à cette partie');
      const betsToRefund = await tx.bet.findMany({ where: { roundId, debitedAt: { not: null }, result: null }, select: { id: true, userId: true, stake: true } });
      await this.lockUsers(tx, [...betsToRefund.map((bet) => bet.userId), userId]);
      const lockedRoom = await tx.room.updateMany({ where: { id: round.roomId, status: 'IN_PROGRESS' }, data: { updatedAt: new Date() } });
      if (lockedRoom.count !== 1) throw new BadRequestException('La partie n’est plus active');
      const locked = await tx.round.updateMany({ where: { id: roundId, status: 'OPEN' }, data: { status: 'FINISHED' } });
      if (locked.count !== 1) return false;
      for (const bet of betsToRefund) {
        await this.ledger.refundBet(tx, bet.userId, bet.stake, `refund:${bet.id}`);
        await tx.bet.update({ where: { id: bet.id }, data: { result: 'CANCELLED', payout: 0 } });
      }
      return true;
    });
  }

  async advanceAfterCancelledRound(gameId: string, roundId: string): Promise<AdvanceAfterRevealResult | null> {
    return this.prisma.$transaction(async (tx) => {
      const game = await tx.game.findUnique({ where: { id: gameId }, include: { rounds: { orderBy: { createdAt: 'asc' }, select: { id: true, status: true } } } });
      if (!game || game.status !== 'IN_PROGRESS') return null;
      const current = game.rounds[game.currentRoundIndex];
      if (!current || current.id !== roundId || current.status !== 'FINISHED') return null;
      const members = await tx.roomMember.findMany({ where: { roomId: game.roomId, state: 'PLAYING' }, select: { userId: true } });
      await this.lockUsers(tx, members.map((m) => m.userId));
      const roomLock = await tx.room.updateMany({ where: { id: game.roomId, status: 'IN_PROGRESS' }, data: { updatedAt: new Date() } });
      if (roomLock.count !== 1) return null;
      if (game.currentRoundIndex >= GAME_ROUNDS - 1) {
        await tx.game.updateMany({ where: { id: gameId, status: 'IN_PROGRESS', currentRoundIndex: game.currentRoundIndex }, data: { status: 'FINISHED', finishedAt: new Date() } });
        await tx.room.updateMany({ where: { id: game.roomId, status: 'IN_PROGRESS' }, data: { status: 'FINISHED' } });
        await tx.roomMember.updateMany({ where: { roomId: game.roomId }, data: { state: 'FINISHED' } });
        await tx.user.updateMany({ where: { activeRoomId: game.roomId }, data: { activeRoomId: null } });
        return { finished: true };
      }
      const next = game.rounds[game.currentRoundIndex + 1];
      if (!next) return null;
      const claim = await tx.roundClaim.findFirst({ where: { roundId: next.id }, include: { claim: { select: { videoEndS: true } } } });
      if (!claim?.claim.videoEndS) return null;
      const now = new Date();
      const locksAt = new Date(now.getTime() + (claim.claim.videoEndS + 30) * 1000);
      await tx.game.updateMany({ where: { id: gameId, status: 'IN_PROGRESS', currentRoundIndex: game.currentRoundIndex }, data: { currentRoundIndex: { increment: 1 } } });
      await tx.round.updateMany({ where: { id: next.id, status: 'COUNTDOWN' }, data: { status: 'OPEN', opensAt: now, locksAt } });
      return { finished: false, roundId: next.id };
    });
  }

  async advanceAfterReveal(gameId: string): Promise<AdvanceAfterRevealResult | null> {
    const game = await this.prisma.game.findUnique({ where: { id: gameId }, include: { rounds: { orderBy: { createdAt: 'asc' }, select: { id: true, status: true, locksAt: true, revealedAt: true } } } });
    if (!game || game.status !== 'IN_PROGRESS') return null;
    const current = game.rounds[game.currentRoundIndex];
    if (!current || current.status !== 'REVEALED') return null;

    const now = new Date();
    let transitionAllowedAt = current.revealedAt
      ? new Date(current.revealedAt.getTime() + RESULT_DISPLAY_MS)
      : now;

    if (transitionAllowedAt.getTime() > now.getTime()) return null;

    if (game.currentRoundIndex >= GAME_ROUNDS - 1) {
      return this.prisma.$transaction(async (tx): Promise<AdvanceAfterRevealResult | null> => {
        const members = await tx.roomMember.findMany({
          where: { roomId: game.roomId },
          select: { userId: true },
        });
        await this.lockUsers(tx, members.map((member) => member.userId));

        const roomLock = await tx.room.updateMany({
          where: { id: game.roomId, status: 'IN_PROGRESS' },
          data: { updatedAt: new Date() },
        });
        if (roomLock.count !== 1) return null;

        const updated = await tx.game.updateMany({ where: { id: gameId, status: 'IN_PROGRESS', currentRoundIndex: GAME_ROUNDS - 1 }, data: { status: 'FINISHED', finishedAt: new Date() } });
        if (updated.count !== 1) return null;
        await tx.round.updateMany({ where: { id: current.id, status: 'REVEALED' }, data: { status: 'FINISHED' } });
        await tx.room.updateMany({ where: { id: game.roomId, status: 'IN_PROGRESS' }, data: { status: 'FINISHED' } });
        await tx.roomMember.updateMany({ where: { roomId: game.roomId }, data: { state: 'FINISHED' } });
        await tx.user.updateMany({ where: { activeRoomId: game.roomId }, data: { activeRoomId: null } });
        return { finished: true };
      });
    }

    const next = game.rounds[game.currentRoundIndex + 1];
    if (!next) return null;
    const claim = await this.prisma.roundClaim.findFirst({ where: { roundId: next.id }, include: { claim: { select: { videoEndS: true } } } });
    if (!claim?.claim.videoEndS) return null;
    const locksAt = new Date(now.getTime() + (claim.claim.videoEndS + 30) * 1000);

    return this.prisma.$transaction(async (tx): Promise<AdvanceAfterRevealResult | null> => {
      const roomLock = await tx.room.updateMany({
        where: { id: game.roomId, status: 'IN_PROGRESS' },
        data: { updatedAt: new Date() },
      });
      if (roomLock.count !== 1) return null;

      const updated = await tx.game.updateMany({ where: { id: gameId, status: 'IN_PROGRESS', currentRoundIndex: game.currentRoundIndex }, data: { currentRoundIndex: { increment: 1 } } });
      if (updated.count !== 1) return null;
      await tx.round.updateMany({ where: { id: current.id, status: 'REVEALED' }, data: { status: 'FINISHED' } });
      await tx.round.updateMany({ where: { id: next.id, status: 'COUNTDOWN' }, data: { status: 'OPEN', opensAt: now, locksAt } });
      return { finished: false, roundId: next.id };
    });
  }

  private async lockUser(tx: Prisma.TransactionClient, userId: string) {
    const locked = await tx.user.updateMany({
      where: { id: userId },
      data: { updatedAt: new Date() },
    });
    if (locked.count !== 1) throw new NotFoundException('Utilisateur introuvable');
  }

  private async lockUsers(tx: Prisma.TransactionClient, userIds: string[]) {
    for (const userId of [...new Set(userIds)].sort()) {
      await this.lockUser(tx, userId);
    }
  }

}
