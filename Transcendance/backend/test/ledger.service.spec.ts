import { jest } from '@jest/globals';
import { BadRequestException } from '@nestjs/common';
import { LedgerService } from '../src/ledger/ledger.service.js';

describe('LedgerService', () => {
  const realtimeEvents = { emitToUser: jest.fn() };

  it('exposes the same recovery eligibility used by the claim endpoint', async () => {
    const prisma = {
      user: {
        findUnique: jest.fn().mockResolvedValue({
          virtualBalance: 0,
          bankruptEpisode: 3,
          recoveredEpisode: 2,
          recoveryEligibleEpisode: 3,
        }),
      },
    };
    const service = new LedgerService(prisma as never, realtimeEvents as never);
    await expect(service.getBalance('user-1')).resolves.toEqual({
      virtualBalance: 0,
      recoveryAvailable: true,
    });
  });

  it('rejects non-positive amounts', async () => {
    const service = new LedgerService({} as never, realtimeEvents as never);
    await expect(service.creditPayout({} as never, 'user-1', 0)).rejects.toBeInstanceOf(BadRequestException);
    await expect(service.debitForBet({} as never, 'user-1', -1)).rejects.toBeInstanceOf(BadRequestException);
  });

  it('does not apply an already recorded payout twice', async () => {
    const existing = { id: 'tx-1', userId: 'user-1', type: 'PAYOUT', amount: 20, referenceId: 'payout:p1' };
    const tx = { transaction: { findUnique: jest.fn().mockResolvedValue(existing), create: jest.fn() }, user: { update: jest.fn() } };
    const service = new LedgerService({} as never, realtimeEvents as never);
    await expect(service.creditPayout(tx as never, 'user-1', 20, 'payout:p1')).resolves.toBe(existing);
    expect(tx.user.update).not.toHaveBeenCalled();
  });

  it('records a penalty of at most 50 and never below zero', async () => {
    const tx = {
      transaction: { findUnique: jest.fn().mockResolvedValue(null), create: jest.fn().mockResolvedValue({}) },
      user: { findUnique: jest.fn().mockResolvedValue({ virtualBalance: 40 }), updateMany: jest.fn().mockResolvedValue({ count: 1 }), update: jest.fn().mockResolvedValue({}), findUniqueOrThrow: jest.fn().mockResolvedValue({ virtualBalance: 0 }) },
    };
    const service = new LedgerService({} as never, realtimeEvents as never);
    await service.debitPenalty(tx as never, 'user-1', 'penalty:test');
    expect(tx.user.updateMany).toHaveBeenCalledWith(expect.objectContaining({ data: { virtualBalance: { decrement: 40 } } }));
    expect(tx.transaction.create).toHaveBeenCalledWith(expect.objectContaining({ data: expect.objectContaining({ type: 'PENALTY', amount: -40, balanceAfter: 0 }) }));
  });

  it('refuses recovery while the bankruptcy episode is not settled', async () => {
    const tx = {
      user: {
        findUnique: jest.fn().mockResolvedValue({
          virtualBalance: 0,
          bankruptEpisode: 1,
          recoveredEpisode: 0,
          recoveryEligibleEpisode: null,
        }),
        updateMany: jest.fn(),
      },
      transaction: { create: jest.fn() },
    };
    const prisma = { $transaction: jest.fn((cb: any) => cb(tx)) };
    const service = new LedgerService(prisma as never, realtimeEvents as never);

    await expect(service.reclamerRattrapage('user-1')).rejects.toThrow(
      'La récupération sera disponible après le règlement de la manche',
    );
    expect(tx.user.updateMany).not.toHaveBeenCalled();
    expect(tx.transaction.create).not.toHaveBeenCalled();
  });

  it('allows recovery only for the settled unrecovered episode', async () => {
    const tx = {
      user: {
        findUnique: jest.fn().mockResolvedValue({
          virtualBalance: 0,
          bankruptEpisode: 2,
          recoveredEpisode: 1,
          recoveryEligibleEpisode: 2,
        }),
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
      },
      transaction: { create: jest.fn().mockResolvedValue({}) },
    };
    const prisma = { $transaction: jest.fn((cb: any) => cb(tx)) };
    const service = new LedgerService(prisma as never, realtimeEvents as never);

    await expect(service.reclamerRattrapage('user-1')).resolves.toMatchObject({
      virtualBalance: 200,
      credite: 200,
      episode: 2,
    });
    expect(tx.user.updateMany).toHaveBeenCalledWith(expect.objectContaining({
      where: expect.objectContaining({ recoveryEligibleEpisode: 2 }),
      data: expect.objectContaining({ recoveryEligibleEpisode: null }),
    }));
    expect(tx.transaction.create).toHaveBeenCalledWith(expect.objectContaining({
      data: expect.objectContaining({ amount: 200, referenceId: 'recovery:user-1:2' }),
    }));
  });

  it('marks an episode recoverable only after the settlement caller reaches zero', async () => {
    const tx = {
      user: {
        findUnique: jest.fn().mockResolvedValue({
          virtualBalance: 0,
          bankruptEpisode: 4,
          recoveredEpisode: 3,
          recoveryEligibleEpisode: null,
        }),
        update: jest.fn().mockResolvedValue({}),
      },
    };
    const service = new LedgerService({} as never, realtimeEvents as never);
    await expect(service.markRecoveryEligible(tx as never, 'user-1')).resolves.toBe(true);
    expect(tx.user.update).toHaveBeenCalledWith({
      where: { id: 'user-1' },
      data: { recoveryEligibleEpisode: 4 },
    });
  });

  it('does not allow a winning payout to become recoverable', async () => {
    const tx = {
      user: {
        findUnique: jest.fn()
          .mockResolvedValueOnce({
            virtualBalance: 22,
            bankruptEpisode: 1,
            recoveredEpisode: 0,
            recoveryEligibleEpisode: null,
          })
          .mockResolvedValueOnce({
            virtualBalance: 22,
            bankruptEpisode: 1,
            recoveredEpisode: 0,
            recoveryEligibleEpisode: null,
          }),
        update: jest.fn(),
      },
    };
    const service = new LedgerService({} as never, realtimeEvents as never);
    await expect(service.markRecoveryEligible(tx as never, 'user-1')).resolves.toBe(false);
    expect(tx.user.update).not.toHaveBeenCalled();
  });

  it('credits exactly 200 once under a double claim race', async () => {
    const user = {
      virtualBalance: 0,
      bankruptEpisode: 1,
      recoveredEpisode: 0,
      recoveryEligibleEpisode: 1,
    };
    const tx = {
      user: {
        findUnique: jest.fn().mockImplementation(async () => ({ ...user })),
        updateMany: jest.fn().mockImplementation(async ({ where, data }: any) => {
          if (
            user.virtualBalance !== where.virtualBalance ||
            user.recoveredEpisode !== where.recoveredEpisode ||
            user.bankruptEpisode !== where.bankruptEpisode ||
            user.recoveryEligibleEpisode !== where.recoveryEligibleEpisode
          ) return { count: 0 };
          user.virtualBalance += data.virtualBalance.increment;
          user.recoveredEpisode = data.recoveredEpisode;
          user.recoveryEligibleEpisode = data.recoveryEligibleEpisode;
          return { count: 1 };
        }),
      },
      transaction: { create: jest.fn().mockResolvedValue({}) },
    };
    const prisma = { $transaction: jest.fn((cb: any) => cb(tx)) };
    const service = new LedgerService(prisma as never, realtimeEvents as never);

    const results = await Promise.allSettled([
      service.reclamerRattrapage('user-1'),
      service.reclamerRattrapage('user-1'),
    ]);

    expect(results.filter((r) => r.status === 'fulfilled')).toHaveLength(1);
    expect(results.filter((r) => r.status === 'rejected')).toHaveLength(1);
    expect(user.virtualBalance).toBe(200);
    expect(tx.transaction.create).toHaveBeenCalledTimes(1);
  });
});
