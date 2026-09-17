import { jest } from '@jest/globals';
import { RealtimeGateway, type AuthenticatedSocket } from '../src/realtime/realtime.gateway.js';

function makeGateway(overrides: Partial<Record<string, any>> = {}) {
  const prisma = {
    room: { findUnique: jest.fn().mockResolvedValue(null) },
    roomMember: { findUnique: jest.fn().mockResolvedValue({ id: 'member-1' }) },
    friend: { findMany: jest.fn().mockResolvedValue([]) },
    user: {
      findUnique: jest.fn().mockResolvedValue({ status: 'OFFLINE' }),
      update: jest.fn().mockResolvedValue({}),
      updateMany: jest.fn().mockResolvedValue({ count: 1 }),
    },
  };
  const presence = {
    leave: jest.fn().mockResolvedValue(null),
    getCurrentRoomId: jest.fn().mockResolvedValue(null),
  };
  const rooms = {
    findOne: jest.fn(),
    leaveLobby: jest.fn(),
    destroyRoomIfEmpty: jest.fn().mockResolvedValue(false),
    getState: jest.fn(),
  };
  const games = { leaveGame: jest.fn() };
  const ledger = { getBalance: jest.fn().mockResolvedValue({ virtualBalance: 100, recoveryAvailable: false }) };
  const gateway = new RealtimeGateway(
    {} as never,
    prisma as never,
    presence as never,
    rooms as never,
    {} as never,
    {} as never,
    {} as never,
    {} as never,
    ledger as never,
    games as never,
  );
  gateway.server = {
    to: jest.fn().mockReturnValue({ emit: jest.fn() }),
    emit: jest.fn(),
    sockets: { sockets: new Map() },
  } as never;
  Object.assign(gateway, overrides);
  return { gateway, prisma, presence, rooms, games, ledger };
}

function socket(): AuthenticatedSocket {
  return {
    id: 'socket-1',
    data: { userId: 'user-a', roomId: 'room-1' },
    leave: jest.fn().mockResolvedValue(undefined),
  } as never;
}

describe('RealtimeGateway room leave concurrency', () => {
  it('rechecks the room after a lost WAITING lock and applies game leave rules', async () => {
    const { gateway, rooms, games } = makeGateway();
    rooms.findOne
      .mockResolvedValueOnce({ id: 'room-1', status: 'WAITING' })
      .mockResolvedValueOnce({ id: 'room-1', status: 'IN_PROGRESS' });
    rooms.leaveLobby.mockResolvedValue(false);
    games.leaveGame.mockResolvedValue({ removed: true, cancelledUserId: undefined });

    await gateway.leave(socket(), { roomId: 'room-1' });

    expect(rooms.leaveLobby).toHaveBeenCalledWith('room-1', 'user-a');
    expect(games.leaveGame).toHaveBeenCalledWith('room-1', 'user-a');
    expect(rooms.destroyRoomIfEmpty).toHaveBeenCalledWith('room-1');
  });

  it('makes a double leave idempotent at the gateway boundary', async () => {
    const { gateway, rooms, games } = makeGateway();
    rooms.findOne.mockResolvedValue({ id: 'room-1', status: 'WAITING' });
    rooms.leaveLobby.mockResolvedValueOnce(true).mockResolvedValueOnce(false);

    await Promise.all([
      gateway.leave(socket(), { roomId: 'room-1' }),
      gateway.leave(socket(), { roomId: 'room-1' }),
    ]);

    expect(rooms.leaveLobby).toHaveBeenCalledTimes(2);
    expect(games.leaveGame).not.toHaveBeenCalled();
  });

  it('keeps disconnect technical when it races with voluntary leave', async () => {
    const { gateway, rooms, presence } = makeGateway();
    rooms.findOne.mockResolvedValue({ id: 'room-1', status: 'WAITING' });
    rooms.leaveLobby.mockResolvedValue(true);
    presence.leave
      .mockResolvedValueOnce({ socketId: 'socket-1', userId: 'user-a', roomId: 'room-1', leftRoom: true })
      .mockResolvedValueOnce(null);

    const s = socket();
    await Promise.all([
      gateway.leave(s, { roomId: 'room-1' }),
      gateway.handleDisconnect(s),
    ]);

    expect(rooms.leaveLobby).toHaveBeenCalledTimes(1);
    expect(presence.leave).toHaveBeenCalledWith('socket-1');
    expect(gateway.server).toBeDefined();
  });
});


describe('RealtimeGateway synchronization protocol', () => {
  it('emits profile invalidations without sending derived statistics', () => {
    const { gateway } = makeGateway();
    gateway.invalidateStats('user-a');
    gateway.invalidateHistory('user-a');
    gateway.invalidateLeaderboard();

    const emit = (gateway.server as any).emit as jest.Mock;
    expect(emit).toHaveBeenNthCalledWith(1, 'stats:invalidate', { userId: 'user-a' });
    expect(emit).toHaveBeenNthCalledWith(2, 'history:invalidate', { userId: 'user-a' });
    expect(emit).toHaveBeenNthCalledWith(3, 'leaderboard:invalidate');
  });

  it('notifies only accepted friends when a presence transition occurs', async () => {
    const { gateway, prisma } = makeGateway();
    prisma.user.findUnique.mockResolvedValue({ status: 'OFFLINE' });
    prisma.friend.findMany.mockResolvedValue([
      { userId: 'user-a', friendId: 'friend-a' },
      { userId: 'other', friendId: 'user-a' },
    ]);
    (gateway as any).socketsByUser.set('user-a', new Set(['socket-a']));
    const userChannel = (gateway as any).userChannel.bind(gateway);

    await gateway.syncUserPresence('user-a');

    expect(prisma.user.updateMany).toHaveBeenCalledWith({
      where: { id: 'user-a', status: { not: 'ONLINE' } },
      data: { status: 'ONLINE' },
    });
    expect((gateway.server as any).to).toHaveBeenCalledWith(userChannel('friend-a'));
    expect((gateway.server as any).to).toHaveBeenCalledWith(userChannel('other'));
    expect((gateway.server as any).to('user:friend-a').emit).toHaveBeenCalledWith(
      'presence:update',
      { userId: 'user-a', status: 'ONLINE', currentRoomId: null },
    );
  });
});


describe('RealtimeGateway presence transitions', () => {
  it.each([
    ['OFFLINE', null, true, 'ONLINE'],
    ['ONLINE', 'room-1', true, 'IN_GAME'],
    ['IN_GAME', null, true, 'ONLINE'],
    ['ONLINE', null, false, 'OFFLINE'],
    ['IN_GAME', null, false, 'OFFLINE'],
  ] as const)('%s -> %s', async (from, roomId, connected, expected) => {
    const { gateway, prisma, presence } = makeGateway();
    prisma.user.findUnique.mockResolvedValue({ status: from });
    prisma.friend.findMany.mockResolvedValue([{ userId: 'user-a', friendId: 'friend-a' }]);
    presence.getCurrentRoomId.mockResolvedValue(roomId);
    if (connected) (gateway as any).socketsByUser.set('user-a', new Set(['socket-a']));

    await gateway.syncUserPresence('user-a');

    expect(prisma.user.updateMany).toHaveBeenCalledWith({
      where: { id: 'user-a', status: { not: expected } },
      data: { status: expected },
    });
    expect((gateway.server as any).to('user:friend-a').emit).toHaveBeenCalledWith(
      'presence:update',
      { userId: 'user-a', status: expected, currentRoomId: roomId },
    );
  });
});
