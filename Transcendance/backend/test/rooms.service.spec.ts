import { jest } from '@jest/globals';
import { BadRequestException } from '@nestjs/common';
import { RoomsService } from '../src/rooms/rooms.service.js';

function transaction(prisma: any, tx: any) {
  prisma.$transaction = jest.fn(async (callback: (client: any) => unknown) => callback(tx));
  return prisma;
}

describe('RoomsService RoomBan', () => {
  it('refuses a reconnect when the room contains a persistent ban', async () => {
    const tx = {
      user: { updateMany: jest.fn().mockResolvedValue({ count: 1 }), findUnique: jest.fn() },
      room: { updateMany: jest.fn().mockResolvedValue({ count: 1 }), findUnique: jest.fn().mockResolvedValue({ id: 'room-1', status: 'WAITING', lobbyExpiresAt: null }) },
      roomBan: { findUnique: jest.fn().mockResolvedValue({ id: 'ban-1' }) },
      roomMember: { findUnique: jest.fn() },
    };
    const prisma = transaction({}, tx);
    const service = new RoomsService(prisma as never, {} as never, {} as never, {} as never);

    await expect(service.join('room-1', 'user-1')).rejects.toBeInstanceOf(BadRequestException);
    expect(tx.roomBan.findUnique).toHaveBeenCalledWith({
      where: { roomId_userId: { roomId: 'room-1', userId: 'user-1' } },
      select: { id: true },
    });
    expect(tx.roomMember.findUnique).not.toHaveBeenCalled();
  });

  it('creates a RoomBan and removes the member atomically when kicking', async () => {
    const tx = {
      user: { updateMany: jest.fn().mockResolvedValue({ count: 1 }), findUnique: jest.fn() },
      room: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({ status: 'WAITING', lobbyExpiresAt: null }),
        update: jest.fn().mockResolvedValue({}),
      },
      roomMember: {
        findUnique: jest.fn()
          .mockResolvedValueOnce({ id: 'requester', userId: 'host', ready: false })
          .mockResolvedValueOnce({ id: 'target', userId: 'player', ready: false, joinedAt: new Date(Date.now() - 60_000) }),
        deleteMany: jest.fn().mockResolvedValue({ count: 1 }),
      },
      roomBan: { upsert: jest.fn().mockResolvedValue({}) },
    };
    const prisma = transaction({}, tx);
    const service = new RoomsService(prisma as never, {} as never, {} as never, {} as never);

    await expect(service.kick('room-1', 'host', 'player')).resolves.toEqual(expect.objectContaining({ targetId: 'player' }));
    expect(tx.roomBan.upsert).toHaveBeenCalledWith({
      where: { roomId_userId: { roomId: 'room-1', userId: 'player' } },
      update: {},
      create: { roomId: 'room-1', userId: 'player' },
    });
    expect(tx.roomMember.deleteMany).toHaveBeenCalledWith({ where: { id: 'target', ready: false } });
  });
});


describe('RoomsService empty-room lifecycle and availability limit', () => {
  it('destroys an empty room in dependency order and ignores RoomPresence', async () => {
    const tx = {
      user: { findMany: jest.fn().mockResolvedValue([]), updateMany: jest.fn().mockResolvedValue({ count: 0 }) },
      room: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({ id: 'room-1', status: 'IN_PROGRESS' }),
        deleteMany: jest.fn().mockResolvedValue({ count: 1 }),
      },
      roomMember: { count: jest.fn().mockResolvedValue(0) },
      message: { deleteMany: jest.fn().mockResolvedValue({ count: 2 }) },
      clipVote: { deleteMany: jest.fn().mockResolvedValue({ count: 3 }) },
      bet: { deleteMany: jest.fn().mockResolvedValue({ count: 4 }) },
      roundClaim: { deleteMany: jest.fn().mockResolvedValue({ count: 5 }) },
      round: { deleteMany: jest.fn().mockResolvedValue({ count: 5 }) },
      game: { deleteMany: jest.fn().mockResolvedValue({ count: 1 }) },
    };
    const prisma = transaction({}, tx);
    const realtimeEvents = {
      ejectRoom: jest.fn().mockResolvedValue(undefined),
      emitToRoom: jest.fn(),
      emitAll: jest.fn(),
    };
    const service = new RoomsService(prisma as never, {} as never, {} as never, realtimeEvents as never);

    await expect(service.destroyRoomIfEmpty('room-1')).resolves.toBe(true);
    expect(tx.message.deleteMany).toHaveBeenCalled();
    expect(tx.clipVote.deleteMany).toHaveBeenCalled();
    expect(tx.bet.deleteMany).toHaveBeenCalled();
    expect(tx.roundClaim.deleteMany).toHaveBeenCalled();
    expect(tx.round.deleteMany).toHaveBeenCalled();
    expect(tx.game.deleteMany).toHaveBeenCalled();
    expect(tx.room.deleteMany).toHaveBeenCalledWith({ where: { id: 'room-1' } });
    expect(realtimeEvents.ejectRoom).toHaveBeenCalledWith('room-1');
  });

  it('preserves finished-game history when the room becomes empty', async () => {
    const tx = {
      user: { findMany: jest.fn().mockResolvedValue([]), updateMany: jest.fn().mockResolvedValue({ count: 0 }) },
      room: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({ id: 'room-finished', status: 'FINISHED' }),
        deleteMany: jest.fn(),
      },
      roomMember: { count: jest.fn().mockResolvedValue(0) },
      message: { deleteMany: jest.fn().mockResolvedValue({ count: 2 }) },
      roomBan: { deleteMany: jest.fn().mockResolvedValue({ count: 1 }) },
      clipVote: { deleteMany: jest.fn() },
      bet: { deleteMany: jest.fn() },
      roundClaim: { deleteMany: jest.fn() },
      round: { deleteMany: jest.fn() },
      game: { deleteMany: jest.fn() },
    };
    const prisma = transaction({}, tx);
    const realtimeEvents = { ejectRoom: jest.fn(), emitToRoom: jest.fn(), emitAll: jest.fn() };
    const service = new RoomsService(prisma as never, {} as never, {} as never, realtimeEvents as never);

    await expect(service.destroyRoomIfEmpty('room-finished')).resolves.toBe(false);
    expect(tx.message.deleteMany).toHaveBeenCalledWith({ where: { roomId: 'room-finished' } });
    expect(tx.roomBan.deleteMany).toHaveBeenCalledWith({ where: { roomId: 'room-finished' } });
    expect(tx.clipVote.deleteMany).not.toHaveBeenCalled();
    expect(tx.bet.deleteMany).not.toHaveBeenCalled();
    expect(tx.round.deleteMany).not.toHaveBeenCalled();
    expect(tx.game.deleteMany).not.toHaveBeenCalled();
    expect(tx.room.deleteMany).not.toHaveBeenCalled();
    expect(realtimeEvents.ejectRoom).not.toHaveBeenCalled();
  });

  it('never destroys a room when a logical RoomMember remains', async () => {
    const tx = {
      user: { findMany: jest.fn().mockResolvedValue([]), updateMany: jest.fn().mockResolvedValue({ count: 0 }) },
      room: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({ id: 'room-1', status: 'WAITING' }),
        deleteMany: jest.fn(),
      },
      roomMember: { count: jest.fn().mockResolvedValue(1) },
    };
    const prisma = transaction({}, tx);
    const realtimeEvents = { ejectRoom: jest.fn(), emitToRoom: jest.fn(), emitAll: jest.fn() };
    const service = new RoomsService(prisma as never, {} as never, {} as never, realtimeEvents as never);

    await expect(service.destroyRoomIfEmpty('room-1')).resolves.toBe(false);
    expect(tx.room.deleteMany).not.toHaveBeenCalled();
  });

  it('refuses creation when five non-expired waiting rooms already exist', async () => {
    const tx = {
      user: { updateMany: jest.fn().mockResolvedValue({ count: 1 }), findUnique: jest.fn().mockResolvedValue({ activeRoomId: null, activeRoom: null }), update: jest.fn() },
      room: {
        count: jest.fn().mockResolvedValue(5),
        create: jest.fn(),
      },
      roomMember: { create: jest.fn() },
    };
    const prisma = transaction({}, tx);
    const realtimeEvents = { emitAll: jest.fn() };
    const service = new RoomsService(prisma as never, {} as never, {} as never, realtimeEvents as never);

    await expect(service.create({ name: 'sixth' }, 'user-1')).rejects.toBeInstanceOf(BadRequestException);
    expect(tx.room.create).not.toHaveBeenCalled();
  });
});




describe('RoomsService creation isolation', () => {
  it('uses PostgreSQL SERIALIZABLE for the availability check and creation', async () => {
    const tx = {
      user: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({ activeRoomId: null, activeRoom: null }),
        update: jest.fn().mockResolvedValue({}),
      },
      room: {
        count: jest.fn().mockResolvedValue(4),
        create: jest.fn().mockResolvedValue({ id: 'room-5' }),
      },
      roomMember: { create: jest.fn().mockResolvedValue({}) },
    };
    let options: any;
    const prisma = {
      $transaction: jest.fn(async (callback: (client: typeof tx) => unknown, transactionOptions: unknown) => {
        options = transactionOptions;
        return callback(tx);
      }),
    };
    const realtimeEvents = { emitAll: jest.fn() };
    const service = new RoomsService(prisma as never, {} as never, {} as never, realtimeEvents as never);

    await expect(service.create({ name: 'fifth' }, 'user-1')).resolves.toEqual({ id: 'room-5' });
    expect(options).toEqual(expect.objectContaining({ isolationLevel: expect.anything() }));
  });
});


describe('RoomsService active-room invariant', () => {
  function serviceWithTransaction(tx: any) {
    const prisma = transaction({}, tx);
    const realtimeEvents = { emitAll: jest.fn() };
    return { service: new RoomsService(prisma as never, {} as never, {} as never, realtimeEvents as never), prisma, realtimeEvents };
  }

  it('refuses to create another room while the user is in a WAITING room', async () => {
    const tx = {
      user: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({
          activeRoomId: 'room-a',
          activeRoom: { id: 'room-a', status: 'WAITING' },
        }),
        update: jest.fn(),
      },
      room: { count: jest.fn(), create: jest.fn() },
      roomMember: { create: jest.fn() },
    };
    const { service } = serviceWithTransaction(tx);

    await expect(service.create({ name: 'B' }, 'user-1')).rejects.toThrow('USER_ALREADY_IN_ROOM');
    expect(tx.room.create).not.toHaveBeenCalled();
  });

  it('allows joining the same active room idempotently', async () => {
    const tx = {
      user: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({
          activeRoomId: 'room-a',
          activeRoom: { id: 'room-a', status: 'WAITING' },
        }),
        update: jest.fn().mockResolvedValue({}),
      },
      room: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({ id: 'room-a', status: 'WAITING', lobbyExpiresAt: null }),
        update: jest.fn().mockResolvedValue({}),
      },
      roomBan: { findUnique: jest.fn().mockResolvedValue(null) },
      roomMember: {
        findUnique: jest.fn().mockResolvedValue({ id: 'member-a', userId: 'user-1' }),
        count: jest.fn(),
        upsert: jest.fn().mockResolvedValue({ id: 'member-a', userId: 'user-1' }),
      },
    };
    const { service } = serviceWithTransaction(tx);

    await expect(service.join('room-a', 'user-1')).resolves.toEqual({ id: 'member-a', userId: 'user-1' });
    expect(tx.user.update).toHaveBeenCalledWith({ where: { id: 'user-1' }, data: { activeRoomId: 'room-a' } });
  });

  it('refuses joining a different room while the user is active elsewhere', async () => {
    const tx = {
      user: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({
          activeRoomId: 'room-a',
          activeRoom: { id: 'room-a', status: 'IN_PROGRESS' },
        }),
      },
      room: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({ id: 'room-b', status: 'WAITING', lobbyExpiresAt: null }),
      },
      roomBan: { findUnique: jest.fn().mockResolvedValue(null) },
      roomMember: { findUnique: jest.fn().mockResolvedValue(null), count: jest.fn() },
    };
    const { service } = serviceWithTransaction(tx);

    await expect(service.join('room-b', 'user-1')).rejects.toThrow('USER_ALREADY_IN_ROOM');
    expect(tx.room.updateMany).toHaveBeenCalledWith({
      where: { id: 'room-b' },
      data: { updatedAt: expect.any(Date) },
    });
  });

  it('releases the active room when leaving the lobby', async () => {
    const tx = {
      user: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findMany: jest.fn().mockResolvedValue([]),
      },
      room: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        update: jest.fn().mockResolvedValue({}),
        findUnique: jest.fn().mockResolvedValue({ id: 'room-a', status: 'WAITING' }),
      },
      roomMember: {
        deleteMany: jest.fn().mockResolvedValue({ count: 1 }),
        count: jest.fn().mockResolvedValue(1),
      },
    };

    const { service } = serviceWithTransaction(tx);

    await expect(service.leaveLobby('room-a', 'user-1')).resolves.toBe(true);

    expect(tx.user.updateMany).toHaveBeenCalledWith({
      where: { id: 'user-1', activeRoomId: 'room-a' },
      data: { activeRoomId: null },
    });
  });

  it('releases a stale FINISHED active room before creating a new one', async () => {
    const tx = {
      user: {
        updateMany: jest.fn().mockResolvedValue({ count: 1 }),
        findUnique: jest.fn().mockResolvedValue({
          activeRoomId: 'room-old',
          activeRoom: { id: 'room-old', status: 'FINISHED' },
        }),
        update: jest.fn().mockResolvedValue({}),
      },
      room: {
        count: jest.fn().mockResolvedValue(0),
        create: jest.fn().mockResolvedValue({ id: 'room-new' }),
      },
      roomMember: { create: jest.fn().mockResolvedValue({}) },
    };
    const { service } = serviceWithTransaction(tx);

    await expect(service.create({ name: 'new' }, 'user-1')).resolves.toEqual({ id: 'room-new' });
    expect(tx.user.update).toHaveBeenNthCalledWith(1, { where: { id: 'user-1' }, data: { activeRoomId: null } });
    expect(tx.user.update).toHaveBeenNthCalledWith(2, { where: { id: 'user-1' }, data: { activeRoomId: 'room-new' } });
    expect(tx.roomMember.create).toHaveBeenCalledWith({
      data: { roomId: 'room-new', userId: 'user-1', state: 'NOT_READY' },
    });
  });
});


describe('RoomsService revealed round state', () => {
  it('exposes transitionEndsAt from revealedAt using the server result-display deadline', async () => {
    const revealedAt = new Date('2026-09-10T00:00:00.000Z');
    const room = {
      id: 'room-1',
      name: 'Test',
      status: 'IN_PROGRESS',
      createdBy: 'user-1',
      lobbyExpiresAt: null,
      game: { id: 'game-1', status: 'IN_PROGRESS', currentRoundIndex: 0, startedAt: revealedAt, finishedAt: null },
      members: [],
      rounds: [{
        id: 'round-1',
        gameId: 'game-1',
        status: 'REVEALED',
        opensAt: new Date(revealedAt.getTime() - 10_000),
        locksAt: new Date(revealedAt.getTime() - 1_000),
        revealedAt,
        createdAt: revealedAt,
        updatedAt: revealedAt,
      }],
    };
    const round = {
      ...room.rounds[0],
      room: { id: 'room-1', name: 'Test', status: 'IN_PROGRESS', createdBy: 'user-1' },
      game: { id: 'game-1', status: 'IN_PROGRESS', currentRoundIndex: 0 },
      roundClaims: [{ id: 'rc-1', roundId: 'round-1', claimId: 'claim-1', orderIndex: 0, claim: { id: 'claim-1', text: 'Claim', truthLabel: 'TRUE' } }],
      bets: [{ id: 'bet-1', userId: 'user-1', roundId: 'round-1', stake: 10, result: 'WIN', answer: 'FACT' }],
      clipVotes: [],
    };
    const prisma = {
      room: { findUnique: jest.fn().mockResolvedValue(room) },
      round: { findFirst: jest.fn().mockResolvedValue(round) },
    };
    const service = new RoomsService(prisma as never, {} as never, {} as never, {} as never);

    const state = await service.getState('room-1');
    expect((state.round as any).transitionEndsAt).toEqual(new Date('2026-09-10T00:00:05.000Z'));
    expect((state.round as any).bets[0].result).toBe('WIN');
  });
});


describe('RoomsService disconnect readiness', () => {
  it('returns a disconnected READY member to NOT_READY while the room is still waiting', async () => {
    const tx = {
      room: { updateMany: jest.fn().mockResolvedValue({ count: 1 }) },
      roomMember: {
        findUnique: jest.fn().mockResolvedValue({ id: 'member-1', state: 'READY' }),
        update: jest.fn().mockResolvedValue({ id: 'member-1', state: 'NOT_READY', ready: false }),
      },
    };
    const prisma = transaction({}, tx);
    const service = new RoomsService(prisma as never, {} as never, {} as never, {} as never);

    await expect(service.setNotReadyOnDisconnect('room-1', 'user-1')).resolves.toBe(true);
    expect(tx.roomMember.update).toHaveBeenCalledWith({
      where: { id: 'member-1' },
      data: { ready: false, state: 'NOT_READY' },
    });
  });
});
