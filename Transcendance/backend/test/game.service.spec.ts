import { jest } from '@jest/globals';

jest.unstable_mockModule('node:crypto', () => ({
  randomInt: jest.fn(() => 0),
}));

const { GameService } = await import('../src/realtime/game.service.js');

describe('GameService lobby selection', () => {
  it('shuffles the candidate pool before selecting clips', async () => {
    const candidates = [
      { id: 'c-1', sourceKey: 'alpha' },
      { id: 'c-2', sourceKey: 'alpha' },
      { id: 'c-3', sourceKey: 'alpha' },
      { id: 'c-4', sourceKey: 'beta' },
      { id: 'c-5', sourceKey: 'beta' },
      { id: 'c-6', sourceKey: 'beta' },
    ];
    const prisma = {
      claim: {
        findMany: jest.fn().mockResolvedValue(candidates),
      },
    };
    const service = new GameService(prisma as never, {} as never, {} as never);



  });
});


describe('GameService single-player cancellation', () => {
  it('cancels the game, removes the last player and refunds only unsettled debited bets', async () => {
    let gameStatus: 'IN_PROGRESS' | 'FINISHED' = 'IN_PROGRESS';
    const tx = {
      user: { updateMany: jest.fn().mockResolvedValue({ count: 1 }) },
      room: { updateMany: jest.fn().mockResolvedValue({ count: 1 }) },
      game: {
        findUnique: jest.fn().mockImplementation(async () => gameStatus === 'IN_PROGRESS' ? { id: 'game-1', status: gameStatus } : { id: 'game-1', status: 'FINISHED' }),
        updateMany: jest.fn().mockImplementation(async () => {
          if (gameStatus !== 'IN_PROGRESS') return { count: 0 };
          gameStatus = 'FINISHED';
          return { count: 1 };
        }),
      },
      roomMember: {
        findMany: jest.fn().mockResolvedValue([{ id: 'member-b', userId: 'user-b' }]),
        deleteMany: jest.fn().mockResolvedValue({ count: 1 }),
      },
      bet: {
        findMany: jest.fn().mockResolvedValue([
          { id: 'bet-open', stake: 50, debitedAt: new Date(), result: null, payout: null },
          { id: 'bet-paid', stake: 25, debitedAt: new Date(), result: 'LOSS', payout: 0 },
          { id: 'bet-undebited', stake: 10, debitedAt: null, result: null, payout: null },
        ]),
      },
      round: { updateMany: jest.fn().mockResolvedValue({ count: 1 }) },
    };
    const prisma = transaction({}, tx);
    const ledger = { refundBet: jest.fn().mockResolvedValue({}) };
    const service = new GameService(prisma as never, ledger as never, {} as never);

    await expect(service.cancelGameIfSinglePlayer('room-1')).resolves.toBe('user-b');
    await expect(service.cancelGameIfSinglePlayer('room-1')).resolves.toBe(false);

    expect(ledger.refundBet).toHaveBeenCalledTimes(1);
    expect(ledger.refundBet).toHaveBeenCalledWith(tx, 'user-b', 50, 'refund:bet-open');
    expect(tx.game.updateMany).toHaveBeenCalledWith(expect.objectContaining({
      where: { id: 'game-1', status: 'IN_PROGRESS' },
    }));
    expect(tx.roomMember.deleteMany).toHaveBeenCalledWith({ where: { id: 'member-b' } });
    expect(tx.room.updateMany).toHaveBeenCalledWith({ where: { id: 'room-1', status: 'IN_PROGRESS' }, data: { status: 'FINISHED' } });
  });
});


describe('GameService reveal transition delay', () => {
  it('does not wait five seconds after the last answeredAt', async () => {
    const base = Date.now();
    const prisma = {
      game: {
        findUnique: jest.fn().mockResolvedValue({
          id: 'game-1',
          roomId: 'room-1',
          status: 'IN_PROGRESS',
          currentRoundIndex: 0,
          rounds: [
            { id: 'round-1', status: 'REVEALED', locksAt: new Date(base - 10_000), revealedAt: new Date(base - 6_000) },
            { id: 'round-2', status: 'COUNTDOWN', locksAt: new Date(base + 30_000), revealedAt: null },
          ],
        }),
      },
      roundClaim: {
        findFirst: jest.fn().mockResolvedValue({ claim: { videoEndS: 10 } }),
      },
      $transaction: jest.fn().mockResolvedValue({ finished: false, roundId: 'round-2' }),
    };
    const service = new GameService(prisma as never, {} as never, {} as never);

    await expect(service.advanceAfterReveal('game-1')).resolves.toEqual({ finished: false, roundId: 'round-2' });
    expect(prisma.$transaction).toHaveBeenCalledTimes(1);
  });

  it('still respects the result display delay', async () => {
    const base = Date.now();
    const prisma = {
      game: {
        findUnique: jest.fn().mockResolvedValue({
          id: 'game-1',
          roomId: 'room-1',
          status: 'IN_PROGRESS',
          currentRoundIndex: 0,
          rounds: [{ id: 'round-1', status: 'REVEALED', locksAt: new Date(base - 10_000), revealedAt: new Date(base - 1_000) }],
        }),
      },
      $transaction: jest.fn(),
    };
    const service = new GameService(prisma as never, {} as never, {} as never);

    await expect(service.advanceAfterReveal('game-1')).resolves.toBeNull();
    expect(prisma.$transaction).not.toHaveBeenCalled();
  });
});


describe('GameService voluntary leave', () => {
  it('penalizes the leaver but never the sole player left behind', async () => {
    const tx = {
      user: { updateMany: jest.fn().mockResolvedValue({ count: 1 }) },
      room: { updateMany: jest.fn().mockResolvedValue({ count: 1 }) },
      roomMember: {
        findUnique: jest.fn().mockResolvedValue({ id: 'member-a', userId: 'user-a' }),
        delete: jest.fn().mockResolvedValue({}),
        findMany: jest.fn().mockResolvedValue([{ id: 'member-b', userId: 'user-b' }]),
        deleteMany: jest.fn().mockResolvedValue({ count: 1 }),
      },
      game: {
        findUnique: jest.fn().mockResolvedValue({ id: 'game-1', status: 'IN_PROGRESS' }),
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
      },
      bet: { findMany: jest.fn().mockResolvedValue([{ id: 'bet-b', stake: 40, debitedAt: new Date(), result: null, payout: null }]) },
      round: { updateMany: jest.fn().mockResolvedValue({ count: 1 }) },
    };
    const prisma = transaction({}, tx);
    const ledger = {
      debitPenalty: jest.fn().mockResolvedValue({}),
      refundBet: jest.fn().mockResolvedValue({}),
    };
    const service = new GameService(prisma as never, ledger as never, {} as never);

    await expect(service.leaveGame('room-1', 'user-a')).resolves.toEqual({
      removed: true,
      cancelledUserId: 'user-b',
    });
    expect(ledger.debitPenalty).toHaveBeenCalledTimes(1);
    expect(ledger.debitPenalty).toHaveBeenCalledWith(tx, 'user-a', 'penalty:leave:room-1:user-a');
    expect(ledger.refundBet).toHaveBeenCalledWith(tx, 'user-b', 40, 'refund:bet-b');
  });
});


function transaction(prisma: any, tx: any) {
  prisma.$transaction = jest.fn(async (callback: (client: any) => unknown) => callback(tx));
  return prisma;
}

function resolutionFixture(options: {
  members: string[];
  answered: string[];
  locksAt: Date;
}) {
  const bets = options.answered.map((userId) => ({
    id: `bet-${userId}`,
    userId,
    stake: 10,
    answer: 'FACT',
  }));
  const tx = {
    round: {
      findUnique: jest.fn().mockResolvedValue({
        id: 'round-1',
        roomId: 'room-1',
        status: 'OPEN',
        locksAt: options.locksAt,
        game: { id: 'game-1', status: 'IN_PROGRESS' },
        roundClaims: [{ claim: { truthLabel: 'TRUE' } }],
        bets,
      }),
      updateMany: jest.fn().mockResolvedValue({ count: 1 }),
      update: jest.fn().mockResolvedValue({}),
    },
    room: { updateMany: jest.fn().mockResolvedValue({ count: 1 }) },
    roomMember: {
      findMany: jest.fn().mockResolvedValue(options.members.map((userId) => ({ userId }))),
    },
    user: {
      updateMany: jest.fn().mockResolvedValue({ count: 1 }),
      findUnique: jest.fn().mockResolvedValue({
        virtualBalance: 0,
        bankruptEpisode: 1,
        recoveredEpisode: 0,
        recoveryEligibleEpisode: null,
      }),
      findMany: jest.fn(async ({ where }: any) => (where?.id?.in ?? []).map((id: string) => ({ id }))),
      update: jest.fn().mockResolvedValue({}),
    },
    bet: {
      update: jest.fn().mockResolvedValue({}),
      count: jest.fn().mockResolvedValue(1),
    },
    achievementUnlock: { createMany: jest.fn().mockResolvedValue({ count: 1 }) },
  };
  const ledger = { creditPayout: jest.fn().mockResolvedValue({}), markRecoveryEligible: jest.fn().mockResolvedValue(false) };
  const prisma = transaction({}, tx);
  return { prisma, tx, ledger };
}

describe('GameService early round resolution', () => {
  it('resolves immediately when 2/2 PLAYING members have answered before locksAt', async () => {
    const { prisma, tx, ledger } = resolutionFixture({
      members: ['alice', 'bob'],
      answered: ['alice', 'bob'],
      locksAt: new Date(Date.now() + 50_000),
    });
    const service = new GameService(prisma as never, ledger as never, {} as never);

    await expect(service.resolveRound('round-1')).resolves.toEqual(expect.objectContaining({ roundId: 'round-1' }));
    expect(tx.round.updateMany).toHaveBeenCalledWith({
      where: { id: 'round-1', status: 'OPEN' },
      data: { status: 'LOCKED' },
    });
    expect(tx.round.update).toHaveBeenCalledWith(expect.objectContaining({ data: expect.objectContaining({ status: 'REVEALED', revealedAt: expect.any(Date) }) }));
  });

  it('resolves immediately when 5/5 PLAYING members have answered before locksAt', async () => {
    const players = ['a', 'b', 'c', 'd', 'e'];
    const { prisma, ledger } = resolutionFixture({ members: players, answered: players, locksAt: new Date(Date.now() + 20_000) });
    const service = new GameService(prisma as never, ledger as never, {} as never);

    await expect(service.resolveRound('round-1')).resolves.toEqual(expect.objectContaining({ roundId: 'round-1' }));
  });

  it('does not resolve early when one PLAYING member has not answered', async () => {
    const { prisma, tx, ledger } = resolutionFixture({
      members: ['a', 'b', 'c', 'd', 'e'],
      answered: ['a', 'b', 'c', 'd'],
      locksAt: new Date(Date.now() + 20_000),
    });
    const service = new GameService(prisma as never, ledger as never, {} as never);

    await expect(service.resolveRound('round-1')).resolves.toBeNull();
    expect(tx.round.updateMany).not.toHaveBeenCalled();
  });

  it('does not let an answered Bet from a former member satisfy allAnswered', async () => {
    const { prisma, tx, ledger } = resolutionFixture({
      members: ['alice', 'bob'],
      answered: ['alice', 'former-bob'],
      locksAt: new Date(Date.now() + 50_000),
    });
    const service = new GameService(prisma as never, ledger as never, {} as never);

    await expect(service.resolveRound('round-1')).resolves.toBeNull();
    expect(tx.round.updateMany).not.toHaveBeenCalled();
  });

  it('resolves at locksAt when a PLAYING member still has no answer', async () => {
    const { prisma, tx, ledger } = resolutionFixture({
      members: ['alice', 'bob'],
      answered: ['alice'],
      locksAt: new Date(Date.now() - 1),
    });
    const service = new GameService(prisma as never, ledger as never, {} as never);

    await expect(service.resolveRound('round-1')).resolves.toEqual(expect.objectContaining({ roundId: 'round-1' }));
    expect(tx.round.updateMany).toHaveBeenCalledWith({ where: { id: 'round-1', status: 'OPEN' }, data: { status: 'LOCKED' } });
  });

  it('uses the Room lock before finalizing an answer', async () => {
    const now = new Date();
    const tx = {
      room: { updateMany: jest.fn().mockResolvedValue({ count: 1 }) },
      round: { findUnique: jest.fn().mockResolvedValue({ id: 'round-1', roomId: 'room-1', status: 'OPEN', locksAt: new Date(now.getTime() + 10_000), roundClaims: [] }) },
      roomMember: { findUnique: jest.fn().mockResolvedValue({ state: 'PLAYING' }) },
      bet: {
        findUnique: jest.fn().mockResolvedValue({ id: 'bet-1', stake: 10, answer: null }),
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUniqueOrThrow: jest.fn().mockResolvedValue({ id: 'bet-1', answer: 'FACT' }),
      },
      user: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({ virtualBalance: 100 }),
      },
    };
    const prisma = transaction({}, tx);
    const ledger = { debitForBet: jest.fn().mockResolvedValue({}) };
    const service = new GameService(prisma as never, ledger as never, {} as never);

    await expect(service.answer('round-1', 'alice', 'FACT' as never)).resolves.toEqual({ id: 'bet-1', answer: 'FACT' });
    expect(tx.user.updateMany.mock.invocationCallOrder[0]).toBeLessThan(tx.room.updateMany.mock.invocationCallOrder[0]);
    expect(tx.room.updateMany.mock.invocationCallOrder[0]).toBeLessThan(tx.round.findUnique.mock.invocationCallOrder[1]);
    expect(ledger.debitForBet).toHaveBeenCalledWith(tx, 'alice', 10, 'bet:bet-1');
  });
});

