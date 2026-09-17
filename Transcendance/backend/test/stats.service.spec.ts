import { jest } from '@jest/globals';
import { StatsService } from '../src/stats/stats.service.js';

describe('StatsService leaderboard', () => {
  it('ranks by score, then by account creation time', async () => {
    const prisma = {
      user: {
        findMany: jest.fn().mockResolvedValue([
          {
            id: 'late-zero',
            email: 'late@example.com',
            displayName: null,
            avatarUrl: null,
            status: 'OFFLINE',
            createdAt: new Date('2026-01-03T00:00:00.000Z'),
            virtualBalance: 1000,
            bets: [],
          },
          {
            id: 'winner',
            email: 'winner@example.com',
            displayName: 'Winner',
            avatarUrl: null,
            status: 'ONLINE',
            createdAt: new Date('2026-01-04T00:00:00.000Z'),
            virtualBalance: 1100,
            bets: [{
              stake: 100,
              payout: 200,
              result: 'WIN',
              createdAt: new Date('2026-01-05T00:00:00.000Z'),
              round: { revealedAt: new Date('2026-01-05T00:00:00.000Z'), createdAt: new Date('2026-01-04T00:00:00.000Z') },
            }],
          },
          {
            id: 'first-zero',
            email: 'first@example.com',
            displayName: null,
            avatarUrl: null,
            status: 'OFFLINE',
            createdAt: new Date('2026-01-01T00:00:00.000Z'),
            virtualBalance: 1000,
            bets: [],
          },
          {
            id: 'second-zero',
            email: 'second@example.com',
            displayName: null,
            avatarUrl: null,
            status: 'OFFLINE',
            createdAt: new Date('2026-01-02T00:00:00.000Z'),
            virtualBalance: 1000,
            bets: [],
          },
        ]),
      },
    };
    const service = new StatsService(prisma as never);

    const classement = await service.getLeaderboard();
    expect(classement.slice(0, 3)).toMatchObject([
      { rank: 1, user: { id: 'winner' }, netGain: 100 },
      { rank: 2, user: { id: 'first-zero' }, netGain: 0 },
      { rank: 3, user: { id: 'second-zero' }, netGain: 0 },
    ]);
    expect(classement).toHaveLength(4);
  });
});


function serviceDataUser(id: string) {
  return { id, email: `${id}@example.com`, displayName: id === 'u1' ? 'Moi' : id, avatarUrl: null, status: 'ONLINE', createdAt: new Date() };
}

describe('StatsService analytics', () => {
  function serviceWith(data: any) {
    const prisma = {
      user: { findUnique: jest.fn().mockResolvedValue(data.user), findMany: jest.fn() },
      bet: { findMany: jest.fn().mockResolvedValue(data.bets ?? []) },
      clipVote: { findMany: jest.fn().mockResolvedValue(data.votes ?? []) },
      transaction: { findMany: jest.fn().mockResolvedValue(data.transactions ?? []), findFirst: jest.fn().mockResolvedValue(data.periodStartTransaction ?? null) },
      friend: { findMany: jest.fn().mockResolvedValue(data.friendships ?? []) },
      room: { findUnique: jest.fn() },
      round: { findMany: jest.fn().mockResolvedValue(data.rounds ?? []) },
    };
    return new StatsService(prisma as never);
  }

  it('calcule la précision par catégorie et transforme null en Général', async () => {
    const service = serviceWith({
      user: { id: 'u1', email: 'u@example.com', displayName: 'U', avatarUrl: null, status: 'ONLINE', createdAt: new Date(), virtualBalance: 1000, bankruptEpisode: 0, recoveredEpisode: 0, recoveryEligibleEpisode: null },
      votes: [
        { id: '1', isFake: false, createdAt: new Date(), claim: { category: null, truthLabel: 'TRUE' } },
        { id: '2', isFake: true, createdAt: new Date(), claim: { category: 'Histoire', truthLabel: 'FALSE' } },
        { id: '3', isFake: false, createdAt: new Date(), claim: { category: 'Histoire', truthLabel: 'FALSE' } },
      ],
    });
    const result = await service.getUserDashboard('u1', { period: 'all' });
    expect(result.accuracy).toMatchObject({
      total: 3, correct: 2, accuracy: 0.6667,
      byCategory: [
        { category: 'Général', total: 1, correct: 1, accuracy: 1 },
        { category: 'Histoire', total: 2, correct: 1, accuracy: 0.5 },
      ],
    });
  });

  it('calcule gains, pertes et meilleur/pire résultat depuis les mises réglées', async () => {
    const service = serviceWith({
      user: { id: 'u1', email: 'u@example.com', displayName: null, avatarUrl: null, status: 'ONLINE', createdAt: new Date(), virtualBalance: 900, bankruptEpisode: 0, recoveredEpisode: 0, recoveryEligibleEpisode: null },
      bets: [
        { stake: 100, payout: 200, result: 'WIN', createdAt: new Date() },
        { stake: 50, payout: 0, result: 'LOSS', createdAt: new Date() },
      ],
    });
    const result = await service.getUserDashboard('u1', { period: 'all' });
    expect(result).toMatchObject({ roundsPlayed: 2, victories: 1, defeats: 1, totalStake: 150, totalWon: 100, totalLost: 50, bestGain: 100, worstLoss: -50, winRate: 0.5 });
  });

  it('rejette une période personnalisée incomplète ou inversée', async () => {
    const service = serviceWith({
      user: { id: 'u1', email: 'u@example.com', displayName: null, avatarUrl: null, status: 'ONLINE', createdAt: new Date(), virtualBalance: 1000, bankruptEpisode: 0, recoveredEpisode: 0, recoveryEligibleEpisode: null },
    });
    await expect(service.getUserDashboard('u1', { period: 'custom', from: '2026-02-01' })).rejects.toThrow();
    await expect(service.getUserDashboard('u1', { period: 'custom', from: '2026-03-01', to: '2026-02-01' })).rejects.toThrow();
  });

  it('ajoute un point de départ de période depuis le ledger sans compter une transaction hors période', async () => {
    const before = {
      id: 'tx-before', type: 'BET', amount: -50, balanceAfter: 950,
      referenceId: 'bet:before', createdAt: new Date('2026-02-28T18:00:00.000Z'),
    };
    const inside = {
      id: 'tx-inside', type: 'PAYOUT', amount: 100, balanceAfter: 1050,
      referenceId: 'payout:inside', createdAt: new Date('2026-03-02T18:00:00.000Z'),
    };
    const service = serviceWith({
      user: { id: 'u1', email: 'u@example.com', displayName: null, avatarUrl: null, status: 'ONLINE', createdAt: new Date(), virtualBalance: 1050, bankruptEpisode: 0, recoveredEpisode: 0, recoveryEligibleEpisode: null },
      transactions: [inside], periodStartTransaction: before,
    });
    const result = await service.getUserDashboard('u1', { period: 'custom', from: '2026-03-01', to: '2026-03-31' });
    expect(result.balanceHistory).toEqual([
      expect.objectContaining({ id: 'period-start:tx-before', type: 'PERIOD_START', amount: 0, balanceAfter: 950, isBaseline: true }),
      expect.objectContaining({ id: 'tx-inside', type: 'PAYOUT', amount: 100, balanceAfter: 1050, isBaseline: false }),
    ]);
  });

  it('refuse une comparaison sans ami accepté et conserve le joueur connecté en première ligne', async () => {
    const service = serviceWith({
      user: { id: 'u1', email: 'u@example.com', displayName: 'Moi', avatarUrl: null, status: 'ONLINE', createdAt: new Date(), virtualBalance: 1000, bankruptEpisode: 0, recoveredEpisode: 0, recoveryEligibleEpisode: null },
      friendships: [],
    });
    await expect(service.getComparisonStats('u1', ['u2'])).rejects.toThrow('ami accepté');

    const friend = { id: 'u2', email: 'ami@example.com', displayName: 'Ami', avatarUrl: null, status: 'ONLINE', createdAt: new Date(), bets: [], clipVotes: [] };
    const prisma = {
      user: { findUnique: jest.fn().mockResolvedValue({ id: 'u1' }), findMany: jest.fn().mockResolvedValue([
        { ...serviceDataUser('u1'), bets: [], clipVotes: [] }, friend,
      ]) },
      bet: { findMany: jest.fn() }, clipVote: { findMany: jest.fn() }, transaction: { findMany: jest.fn(), findFirst: jest.fn() },
      friend: { findMany: jest.fn().mockResolvedValue([{ userId: 'u1', friendId: 'u2' }]) }, room: { findUnique: jest.fn() },
    };
    const comparisonService = new StatsService(prisma as never);
    const result = await comparisonService.getComparisonStats('u1', ['u2']);
    expect(result.map((entry) => entry.user.id)).toEqual(['u1', 'u2']);
  });

  it('utilise le ledger pour M0 et les soldes de fin de manche', async () => {
    const user = { id: 'u1', email: 'u@example.com', displayName: 'U', avatarUrl: null, status: 'ONLINE', createdAt: new Date() };
    const joinedAt = new Date('2026-03-01T11:00:00Z');
    const room = {
      id: 'room1', name: 'Test', status: 'FINISHED', createdAt: new Date('2026-03-01T10:00:00Z'),
      game: { status: 'FINISHED', currentRoundIndex: 0, startedAt: joinedAt, finishedAt: new Date('2026-03-01T12:00:00Z') },
      members: [{ userId: 'u1', joinedAt, user }],
      rounds: [{
        id: 'r1', status: 'FINISHED', opensAt: new Date('2026-03-01T11:00:00Z'), createdAt: joinedAt, revealedAt: new Date('2026-03-01T11:30:00Z'),
        roundClaims: [{ claim: { category: 'Général' } }],
        bets: [{ id: 'b1', userId: 'u1', stake: 100, payout: 50, result: 'LOSS', answer: 'FACT', createdAt: new Date('2026-03-01T11:10:00Z'), user }],
        clipVotes: [],
      }],
    };
    const transactions = [
      { id: 'tx0', userId: 'u1', type: 'BET', amount: -100, balanceAfter: 900, referenceId: 'bet:before-room', createdAt: new Date('2026-02-28T12:00:00Z') },
      { id: 'tx1', userId: 'u1', type: 'BET', amount: -100, balanceAfter: 900, referenceId: 'bet:b1', createdAt: new Date('2026-03-01T11:10:01Z') },
      { id: 'tx2', userId: 'u1', type: 'PAYOUT', amount: 50, balanceAfter: 950, referenceId: 'payout:b1', createdAt: new Date('2026-03-01T11:30:01Z') },
    ];
    const prisma = {
      room: { findUnique: jest.fn().mockResolvedValue(room) },
      roomMember: { findUnique: jest.fn().mockResolvedValue({ id: 'membership' }) },
      transaction: { findMany: jest.fn().mockResolvedValue(transactions) },
    };
    const service = new StatsService(prisma as never);
    const result = await service.getRoomStats('room1', 'u1');
    expect(result.players[0]).toMatchObject({ initialBalance: 900 });
    expect(result.evolution[0]).toMatchObject({ round: 0, u1: 900, status: 'BASELINE' });
    expect(result.evolution[1]).toMatchObject({ round: 1, u1: 950, status: 'FINISHED' });
  });

  it('calcule les statistiques de room à partir des paris et ClipVote, même avec des données partielles', async () => {
    const user = { id: 'u1', email: 'u@example.com', displayName: 'U', avatarUrl: null, status: 'ONLINE', createdAt: new Date() };
    const room = {
      id: 'room1', name: 'Test', status: 'IN_PROGRESS', createdAt: new Date(),
      game: { status: 'IN_PROGRESS', currentRoundIndex: 1, startedAt: new Date(), finishedAt: null },
      members: [{ userId: 'u1', joinedAt: new Date(), user }],
      rounds: [{
        id: 'r1', status: 'OPEN', createdAt: new Date('2026-03-01T12:00:00Z'), revealedAt: null,
        roundClaims: [{ claim: { category: null } }],
        bets: [{ userId: 'u1', stake: 50, payout: null, result: null, answer: null, createdAt: new Date(), user }],
        clipVotes: [{ userId: 'u1', claimId: 'c1', isFake: false, createdAt: new Date(), claim: { text: 'Test claim', mediaRef: 'media', category: null, truthLabel: 'TRUE' }, user }],
      }],
    };
    const prisma = {
      room: { findUnique: jest.fn().mockResolvedValue(room) },
      roomMember: { findUnique: jest.fn().mockResolvedValue({ id: 'membership' }) },
      transaction: { findMany: jest.fn().mockResolvedValue([]) },
    };
    const service = new StatsService(prisma as never);
    const result = await service.getRoomStats('room1', 'u1');
    expect(result.players).toMatchObject([{ user: { id: 'u1' }, roundsPlayed: 0, victories: 0, accuracy: 1, accuracyAnswered: 1, totalStake: 50 }]);
    expect(result.roundDetails[0].clips[0]).toMatchObject({ clip: 'Test claim', category: 'Général', vote: 'FACT', correct: true });
    expect(result.evolution[0]).toMatchObject({ round: 0, u1: 1000, status: 'BASELINE' });
  });

});
