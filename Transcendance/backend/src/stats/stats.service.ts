import { Injectable, NotFoundException, BadRequestException } from '@nestjs/common';
import { PredictionResult, Prisma } from '@prisma/client';
import { PrismaService } from '../prisma/prisma.service.js';
import { isRecoveryAvailable } from '../ledger/recovery-rules.js';

type UserSummary = {
  id: string;
  email: string;
  displayName: string | null;
  avatarUrl: string | null;
  status: string;
  createdAt: Date;
};

type PredictionForStats = {
  stake: number;
  payout: number | null;
  result: PredictionResult | null;
};

type BetForLeaderboard = PredictionForStats & {
  createdAt: Date;
  round: { revealedAt: Date | null; createdAt: Date };
};

type Period = { from?: Date; to?: Date; key: string; label: string };

type AccuracyStats = {
  total: number;
  correct: number;
  accuracy: number;
  byCategory: { category: string; total: number; correct: number; accuracy: number }[];
};

type ComputedStats = {
  roundsPlayed: number;
  victories: number;
  defeats: number;
  totalStake: number;
  totalPayout: number;
  netGain: number;
  totalWon: number;
  totalLost: number;
  bestGain: number;
  worstLoss: number;
  winRate: number;
};

@Injectable()
export class StatsService {
  constructor(private readonly prisma: PrismaService) {}

  async getLeaderboard() {
    const users = await this.prisma.user.findMany({
      where: { role: { not: 'BOT' } },
      select: {
        ...this.userSelect(),
        virtualBalance: true,
        achievements: { select: { achievementKey: true, unlockedAt: true }, orderBy: { unlockedAt: 'asc' } },
        bets: {
          where: { result: { not: null } },
          select: {
            stake: true, payout: true, result: true, createdAt: true,
            round: { select: { revealedAt: true, createdAt: true } },
          },
        },
      },
    });

    return users
      .map((user) => {
        const { bets, virtualBalance, ...profile } = user;
        return {
          rank: 0, user: profile, virtualBalance,
          ...this.computeStats(bets),
          scoreReachedAt: this.scoreReachedAt(profile.createdAt, bets),
        };
      })
      .sort((left, right) =>
        right.netGain - left.netGain ||
        left.scoreReachedAt.getTime() - right.scoreReachedAt.getTime() ||
        left.user.id.localeCompare(right.user.id),
      )
      .map(({ scoreReachedAt: _scoreReachedAt, ...entry }, index) => ({ ...entry, rank: index + 1 }));
  }

  async getUserStats(userId: string) {
    const user = await this.prisma.user.findUnique({
      where: { id: userId },
      select: {
        ...this.userSelect(), virtualBalance: true, bankruptEpisode: true,
        recoveredEpisode: true, recoveryEligibleEpisode: true,
        achievements: { select: { achievementKey: true, unlockedAt: true }, orderBy: { unlockedAt: 'asc' } },
        bets: { where: { result: { not: null } }, select: { stake: true, payout: true, result: true } },
      },
    });
    if (!user) throw new NotFoundException(`Utilisateur ${userId} introuvable`);
    const { bets, virtualBalance, bankruptEpisode, recoveredEpisode, recoveryEligibleEpisode, ...profile } = user;
    return {
      user: profile, virtualBalance,
      recoveryAvailable: isRecoveryAvailable({ virtualBalance, bankruptEpisode, recoveredEpisode, recoveryEligibleEpisode }),
      ...this.computeStats(bets),
    };
  }

  async getUserDashboard(userId: string, periodQuery?: { period?: string; from?: string; to?: string }) {
    const user = await this.prisma.user.findUnique({
      where: { id: userId },
      select: { ...this.userSelect(), virtualBalance: true, bankruptEpisode: true, recoveredEpisode: true, recoveryEligibleEpisode: true },
    });
    if (!user) throw new NotFoundException(`Utilisateur ${userId} introuvable`);
    const period = this.resolvePeriod(periodQuery);
    const dateFilter = period.from || period.to
      ? { createdAt: { ...(period.from ? { gte: period.from } : {}), ...(period.to ? { lte: period.to } : {}) } }
      : undefined;
    const resultDateFilter = period.from || period.to
      ? { revealedAt: { ...(period.from ? { gte: period.from } : {}), ...(period.to ? { lte: period.to } : {}) } }
      : undefined;

    const [bets, votes, transactions, periodStartTransaction, rounds] = await Promise.all([
      this.prisma.bet.findMany({
        where: { userId, result: { not: null }, ...(resultDateFilter ? { round: resultDateFilter } : {}) },
        select: { stake: true, payout: true, result: true, createdAt: true },
        orderBy: { createdAt: 'asc' },
      }),
      this.prisma.clipVote.findMany({
        where: { userId, ...(dateFilter ? dateFilter : {}) },
        select: { id: true, isFake: true, createdAt: true, claim: { select: { category: true, truthLabel: true } } },
        orderBy: { createdAt: 'asc' },
      }),
      this.prisma.transaction.findMany({
        where: { userId, ...(dateFilter ? dateFilter : {}) },
        orderBy: { createdAt: 'asc' },
        select: { id: true, type: true, amount: true, balanceAfter: true, referenceId: true, createdAt: true },
      }),
      period.from
        ? this.prisma.transaction.findFirst({
            where: { userId, createdAt: { lt: period.from } },
            orderBy: { createdAt: 'desc' },
            select: { id: true, type: true, amount: true, balanceAfter: true, referenceId: true, createdAt: true },
          })
        : Promise.resolve(null),
      this.prisma.round.findMany({
        where: {
          status: { in: ['REVEALED', 'FINISHED'] as any },
          revealedAt: { not: null, ...(period.from ? { gte: period.from } : {}), ...(period.to ? { lte: period.to } : {}) },
          bets: { some: { userId } },
        },
        orderBy: [{ revealedAt: 'asc' }, { createdAt: 'asc' }],
        select: { id: true, revealedAt: true, createdAt: true },
      }),
    ]);

    const computed = this.computeStats(bets);
    const accuracy = this.computeAccuracy(votes);
    const balanceHistory = [
      ...(periodStartTransaction ? [{
        id: `period-start:${periodStartTransaction.id}`,
        date: period.from!.toISOString(),
        amount: 0,
        balanceAfter: periodStartTransaction.balanceAfter,
        type: 'PERIOD_START',
        referenceId: periodStartTransaction.referenceId,
        isBaseline: true,
      }] : []),
      ...transactions.map((tx) => ({
        id: tx.id, date: tx.createdAt.toISOString(), amount: tx.amount,
        balanceAfter: tx.balanceAfter, type: tx.type, referenceId: tx.referenceId,
        isBaseline: false,
      })),
    ];
    const transactionPoints = [
      ...(periodStartTransaction ? [periodStartTransaction] : []),
      ...transactions,
    ];
    const roundBalanceHistory = [
    {
      id: 'round-0',
      round: 0,
      date: period.from?.toISOString() ?? new Date().toISOString(),
      balanceAfter: 1000,
    },
    ...rounds.map((round, index) => {
      const at = round.revealedAt ?? round.createdAt;
      const latest = transactionPoints.reduce<typeof transactionPoints[number] | null>((found, tx) => {
        if (tx.createdAt.getTime() > at.getTime()) return found;
        if (!found || tx.createdAt.getTime() > found.createdAt.getTime()) return tx;
        return found;
      }, null);

      return {
        id: round.id,
        round: index + 1,
        date: at.toISOString(),
        balanceAfter: latest?.balanceAfter ?? (periodStartTransaction?.balanceAfter ?? virtualBalance),
      };
    }),
  ];

    const { virtualBalance, bankruptEpisode, recoveredEpisode, recoveryEligibleEpisode, ...profile } = user;
    return {
      user: profile,
      period,
      currentBalance: virtualBalance,
      recoveryAvailable: isRecoveryAvailable({ virtualBalance, bankruptEpisode, recoveredEpisode, recoveryEligibleEpisode }),
      ...computed,
      accuracy,
      balanceHistory,
      roundBalanceHistory,
      generatedAt: new Date().toISOString(),
    };
  }

  async getComparisonStats(userId: string, friendIds: string[]) {
    const ids = [...new Set(friendIds.filter(Boolean))].filter((id) => id !== userId).slice(0, 20);
    const friendships = await this.prisma.friend.findMany({
      where: {
        OR: [
          { userId, friendId: { in: ids }, status: 'ACCEPTED' },
          { friendId: userId, userId: { in: ids }, status: 'ACCEPTED' },
        ],
      },
      select: { userId: true, friendId: true },
    });
    const allowed = new Set<string>();
    friendships.forEach((f) => allowed.add(f.userId === userId ? f.friendId : f.userId));
    const validFriendIds = ids.filter((id) => allowed.has(id));
    if (validFriendIds.length === 0) {
      throw new BadRequestException('Sélectionnez au moins un ami accepté pour comparer.');
    }
    const selected = [userId, ...validFriendIds];

    const users = await this.prisma.user.findMany({
      where: { id: { in: selected } },
      select: {
        ...this.userSelect(),
        virtualBalance: true,
        bets: { where: { result: { not: null } }, select: { result: true, stake: true, payout: true } },
        clipVotes: { select: { isFake: true, claim: { select: { category: true, truthLabel: true } } } },
      },
    });
    const byId = new Map(users.map((u) => [u.id, u]));
    return selected.flatMap((id) => {
      const u = byId.get(id);
      if (!u) return [];
      const computed = this.computeStats(u.bets);
      const accuracy = this.computeAccuracy(u.clipVotes);
      return [{
        user: u,
        winRate: computed.winRate,
        accuracy: accuracy.accuracy,
        accuracyAnswered: accuracy.total,
        victories: computed.victories,
        roundsPlayed: computed.roundsPlayed,
        virtualBalance: u.virtualBalance,
      }];
    });
  }

  async getRoomStats(roomId: string, requesterId?: string) {
    const room = await this.prisma.room.findUnique({
      where: { id: roomId },
      select: {
        id: true, name: true, status: true, createdAt: true,
        game: { select: { status: true, currentRoundIndex: true, startedAt: true, finishedAt: true } },
        members: { select: { userId: true, joinedAt: true, user: { select: this.userSelect() } } },
        rounds: {
          orderBy: { createdAt: 'asc' },
          select: {
            id: true, status: true, opensAt: true, createdAt: true, revealedAt: true,
            roundClaims: { select: { claim: { select: { category: true } } } },
            bets: { select: { id: true, userId: true, stake: true, payout: true, result: true, answer: true, createdAt: true, user: { select: this.userSelect() } } },
            clipVotes: { select: { userId: true, claimId: true, isFake: true, claim: { select: { text: true, mediaRef: true, category: true, truthLabel: true } }, createdAt: true, user: { select: this.userSelect() } } },
          },
        },
      },
    });
    if (!room) throw new NotFoundException(`Salon ${roomId} introuvable`);
    if (requesterId) {
      const member = await this.prisma.roomMember.findUnique({ where: { roomId_userId: { roomId, userId: requesterId } }, select: { id: true } });
      if (!member) throw new NotFoundException('Vous ne faites plus partie de ce salon');
    }

    const memberJoinedAt = new Map(room.members.map(m => [m.userId, m.joinedAt]));
    const participantIds = new Set<string>(room.members.map(m => m.userId));
    room.rounds.forEach(r => { r.bets.forEach(b => participantIds.add(b.userId)); r.clipVotes.forEach(v => participantIds.add(v.userId)); });
    const transactions = participantIds.size ? await this.prisma.transaction.findMany({
      where: { userId: { in: [...participantIds] } },
      orderBy: [{ createdAt: 'asc' }, { id: 'asc' }],
      select: { id: true, userId: true, type: true, amount: true, balanceAfter: true, referenceId: true, createdAt: true },
    }) : [];

    const initialBalances = new Map<string, number>();
    participantIds.forEach(userId => {
      const anchor = memberJoinedAt.get(userId) ?? room.rounds
        .flatMap(r => r.bets.filter(b => b.userId === userId).map(b => b.createdAt))
        .sort((a, b) => a.getTime() - b.getTime())[0];
      if (!anchor) { initialBalances.set(userId, 1000); return; }
      const before = transactions
        .filter(t => t.userId === userId && t.createdAt.getTime() < anchor.getTime())
        .at(-1);
      initialBalances.set(userId, before?.balanceAfter ?? 1000);
    });

    const roomTransactionIdsByRound = new Map<string, Set<string>>();
    room.rounds.forEach(round => roomTransactionIdsByRound.set(round.id, new Set<string>()));
    room.rounds.forEach((round, roundIndex) => {
      const references = new Set<string>();
      round.bets.forEach(bet => {
        references.add(`bet:${bet.id}`);
        references.add(`payout:${bet.id}`);
        references.add(`refund:${bet.id}`);
      });
      const penaltyPrefix = `penalty:absence:${round.id}:`;
      transactions.forEach(t => {
        const ref = t.referenceId ?? '';
        if (references.has(ref) || ref.startsWith(penaltyPrefix)) {
          roomTransactionIdsByRound.get(round.id)!.add(t.id);
          return;
        }
        if (!ref.startsWith(`penalty:leave:${room.id}:`)) return;
        const at = t.createdAt.getTime();
        const opensAt = round.opensAt.getTime();
        const next = room.rounds[roundIndex + 1];
        const nextOpensAt = next?.opensAt.getTime() ?? Number.POSITIVE_INFINITY;
        if (at >= opensAt && at < nextOpensAt) roomTransactionIdsByRound.get(round.id)!.add(t.id);
      });
    });

    const players = new Map<string, {
      user: UserSummary; roundsPlayed: number; victories: number; totalStake: number;
      totalWon: number; totalLost: number; accuracyTotal: number; accuracyCorrect: number;
    }>();
    const ensurePlayer = (userId: string, user: UserSummary) => {
      if (!players.has(userId)) players.set(userId, {
        user, roundsPlayed: 0, victories: 0, totalStake: 0,
        totalWon: 0, totalLost: 0, accuracyTotal: 0, accuracyCorrect: 0,
      });
    };
    room.members.forEach((m) => ensurePlayer(m.userId, m.user));

    const roundEvolution = room.rounds
      .filter((round) => round.status !== 'COUNTDOWN')
      .map((round, index) => {
      const deltas = new Map<string, number>();
      round.bets.forEach((bet) => {
        ensurePlayer(bet.userId, bet.user);
        const p = players.get(bet.userId)!;
        if (bet.result !== 'CANCELLED') p.totalStake += bet.stake;
        if (bet.result && bet.result !== 'CANCELLED') {
          p.roundsPlayed += 1;
          if (bet.result === 'WIN') p.victories += 1;
          const delta = (bet.payout ?? 0) - bet.stake;
          if (delta >= 0) p.totalWon += delta; else p.totalLost += Math.abs(delta);
          deltas.set(bet.userId, (deltas.get(bet.userId) ?? 0) + delta);
        }
      });
      round.clipVotes.forEach((vote) => {
        ensurePlayer(vote.userId, vote.user);
        const p = players.get(vote.userId)!;
        p.accuracyTotal += 1;
        const truthIsFake = vote.claim.truthLabel === 'FALSE';
        if (vote.isFake === truthIsFake) p.accuracyCorrect += 1;
      });
      return { index: index + 1, roundId: round.id, status: round.status, date: (round.revealedAt ?? round.createdAt).toISOString(), deltas };
    });

    const byPlayer = [...players.values()].map((p) => ({
      user: p.user,
      roundsPlayed: p.roundsPlayed,
      victories: p.victories,
      winRate: p.roundsPlayed ? Number((p.victories / p.roundsPlayed).toFixed(4)) : 0,
      totalStake: p.totalStake,
      totalWon: p.totalWon,
      totalLost: p.totalLost,
      netGain: p.totalWon - p.totalLost,
      accuracy: p.accuracyTotal ? Number((p.accuracyCorrect / p.accuracyTotal).toFixed(4)) : 0,
      accuracyAnswered: p.accuracyTotal,
    })).sort((a, b) => b.netGain - a.netGain || b.accuracy - a.accuracy || a.user.id.localeCompare(b.user.id));

    const balances = new Map(initialBalances);
    const evolution: Record<string, unknown>[] = [{ round: 0, roundId: null, status: 'BASELINE', date: room.createdAt.toISOString(), ...Object.fromEntries(balances) }];
    room.rounds
      .filter(round => round.status !== 'COUNTDOWN' && (round.status === 'REVEALED' || round.status === 'FINISHED'))
      .forEach((round, index) => {
        const roundTxIds = roomTransactionIdsByRound.get(round.id) ?? new Set<string>();
        const roundTransactions = transactions
          .filter(t => roundTxIds.has(t.id))
          .sort((a, b) => a.createdAt.getTime() - b.createdAt.getTime() || a.id.localeCompare(b.id));
        const latestByUser = new Map<string, typeof transactions[number]>();
        roundTransactions.forEach(tx => latestByUser.set(tx.userId, tx));
        latestByUser.forEach((tx, userId) => balances.set(userId, tx.balanceAfter));
        evolution.push({
          round: index + 1,
          roundId: round.id,
          status: round.status,
          date: (round.revealedAt ?? round.createdAt).toISOString(),
          ...Object.fromEntries(balances),
        });
      });

    const roundDetails = room.rounds.map((round, index) => ({
      round: index + 1,
      roundId: round.id,
      date: (round.revealedAt ?? round.createdAt).toISOString(),
      status: round.status,
      bets: round.bets.map(b => ({
        userId: b.userId,
        player: b.user.displayName || b.user.email,
        stake: b.stake,
        answer: b.answer,
        result: b.result,
        payout: b.payout,
        variation: b.result ? (b.payout ?? 0) - b.stake : null,
      })),
      clips: round.clipVotes.map(v => ({
        clip: v.claim.text || v.claim.mediaRef || v.claimId,
        category: v.claim.category?.trim() || 'Général',
        claimId: v.claimId,
        player: v.user.displayName || v.user.email,
        vote: v.isFake ? 'FAKE' : 'FACT',
        correct: v.isFake === (v.claim.truthLabel === 'FALSE'),
      })),
    }));
    return {
      room: { id: room.id, name: room.name, status: room.status, createdAt: room.createdAt, game: room.game },
      players: byPlayer.map(p => ({ ...p, initialBalance: initialBalances.get(p.user.id) ?? 1000 })),
      evolution,
      roundDetails,
      roundCount: room.rounds.length,
      generatedAt: new Date().toISOString(),
    };
  }

  async getRoundHistory() {
    return this.prisma.round.findMany({
      where: { status: { in: ['REVEALED', 'FINISHED'] as any } },
      orderBy: [{ revealedAt: 'desc' }, { createdAt: 'desc' }],
      select: this.roundHistorySelect(),
    });
  }

  async getUserRoundHistory(userId: string) {
    const user = await this.prisma.user.findUnique({ where: { id: userId }, select: { id: true } });
    if (!user) throw new NotFoundException(`Utilisateur ${userId} introuvable`);
    return this.prisma.bet.findMany({
      where: { userId, result: { not: null } },
      orderBy: [{ round: { revealedAt: 'desc' } }, { createdAt: 'desc' }],
      select: {
        id: true, stake: true, payout: true, result: true, createdAt: true,
        round: { select: this.roundHistorySelect() },
      },
    });
  }

  private resolvePeriod(input?: { period?: string; from?: string; to?: string }): Period {
    const key = input?.period ?? 'all';
    const now = new Date();
    if (key === 'custom') {
      if (!input?.from || !input?.to) throw new BadRequestException('Une période personnalisée nécessite deux dates');
      const from = new Date(`${input.from}T00:00:00.000Z`);
      const to = new Date(`${input.to}T23:59:59.999Z`);
      if (Number.isNaN(from.getTime()) || Number.isNaN(to.getTime()) || from > to) throw new BadRequestException('Période invalide');
      return { key, label: `${input.from} → ${input.to}`, from, to };
    }
    const months: Record<string, number> = { '1m': 1, '3m': 3, '6m': 6, '1y': 12 };
    if (key === 'all') return { key, label: 'Tout' };
    if (!(key in months)) throw new BadRequestException('Période invalide');
    const from = new Date(now);
    from.setUTCMonth(from.getUTCMonth() - months[key]);
    return { key, label: `Derniers ${months[key]} mois`, from, to: now };
  }

  private computeAccuracy(votes: Array<{ isFake: boolean; claim: { category: string | null; truthLabel: string } }>): AccuracyStats {
    const groups = new Map<string, { total: number; correct: number }>();
    for (const vote of votes) {
      const category = vote.claim.category?.trim() || 'Général';
      const group = groups.get(category) ?? { total: 0, correct: 0 };
      group.total += 1;
      if (vote.isFake === (vote.claim.truthLabel === 'FALSE')) group.correct += 1;
      groups.set(category, group);
    }
    const byCategory = [...groups.entries()].sort(([a], [b]) => a.localeCompare(b, 'fr')).map(([category, g]) => ({
      category, total: g.total, correct: g.correct, accuracy: g.total ? Number((g.correct / g.total).toFixed(4)) : 0,
    }));
    const total = votes.length;
    const correct = [...groups.values()].reduce((s, g) => s + g.correct, 0);
    return { total, correct, accuracy: total ? Number((correct / total).toFixed(4)) : 0, byCategory };
  }

  private computeStats(bets: PredictionForStats[]): ComputedStats {
    const settledBets = bets.filter((bet) => bet.result !== 'CANCELLED');
    const roundsPlayed = settledBets.length;
    const victories = settledBets.filter((bet) => bet.result === 'WIN').length;
    const defeats = settledBets.filter((bet) => bet.result === 'LOSS').length;
    const totalStake = settledBets.reduce((total, bet) => total + bet.stake, 0);
    const deltas = settledBets.map((bet) => (bet.payout ?? 0) - bet.stake);
    const totalPayout = settledBets.reduce((total, bet) => total + (bet.payout ?? 0), 0);
    const netGain = totalPayout - totalStake;
    const totalWon = deltas.filter((d) => d > 0).reduce((s, d) => s + d, 0);
    const totalLost = deltas.filter((d) => d < 0).reduce((s, d) => s + Math.abs(d), 0);
    const bestGain = deltas.length ? Math.max(0, ...deltas) : 0;
    const worstLoss = deltas.length ? Math.min(0, ...deltas) : 0;
    const winRate = roundsPlayed > 0 ? Number((victories / roundsPlayed).toFixed(4)) : 0;
    return { roundsPlayed, victories, defeats, totalStake, totalPayout, netGain, totalWon, totalLost, bestGain, worstLoss, winRate };
  }

  private scoreReachedAt(createdAt: Date, bets: BetForLeaderboard[]): Date {
    const netGain = this.computeStats(bets).netGain;
    let runningScore = 0;
    let reachedAt = createdAt;
    const orderedBets = [...bets].sort((left, right) => {
      const leftDate = left.round.revealedAt ?? left.round.createdAt;
      const rightDate = right.round.revealedAt ?? right.round.createdAt;
      return leftDate.getTime() - rightDate.getTime() || left.createdAt.getTime() - right.createdAt.getTime();
    });
    for (const bet of orderedBets) {
      runningScore += (bet.payout ?? 0) - bet.stake;
      if (runningScore === netGain) {
        reachedAt = bet.round.revealedAt ?? bet.round.createdAt;
        break;
      }
    }
    return reachedAt;
  }

  private userSelect(): Record<keyof UserSummary, true> {
    return { id: true, email: true, displayName: true, avatarUrl: true, status: true, createdAt: true };
  }

  private roundHistorySelect(): Prisma.RoundSelect {
    return {
      id: true, gameId: true, status: true, opensAt: true, locksAt: true, revealedAt: true, createdAt: true, updatedAt: true,
      room: { select: { id: true, name: true, status: true } },
      bets: {
        select: {
          id: true, userId: true, stake: true, payout: true, result: true, answer: true,
          amountConfirmedAt: true, answeredAt: true, createdAt: true,
          user: { select: { id: true, email: true, displayName: true, avatarUrl: true, status: true } },
        },
        orderBy: [{ result: 'desc' }, { payout: 'desc' }, { createdAt: 'asc' }],
      },
    };
  }
}
