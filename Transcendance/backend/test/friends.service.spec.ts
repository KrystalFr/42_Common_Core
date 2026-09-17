import { jest } from '@jest/globals';
import { FriendsService } from '../src/friends/friends.service.js';

function makeService() {
  const prisma: any = {
    user: { findUnique: jest.fn() },
    friend: {
      findUnique: jest.fn(),
      findFirst: jest.fn(),
      findMany: jest.fn(),
      create: jest.fn(),
      update: jest.fn(),
      upsert: jest.fn(),
      deleteMany: jest.fn(),
    },
    $transaction: jest.fn(async (callback: (tx: unknown) => unknown) => callback(prisma)),
  };
  const roomPresence = { getCurrentRoomId: jest.fn().mockResolvedValue(null) };
  const realtimeEvents = { emitToUser: jest.fn() };
  return {
    service: new FriendsService(prisma as never, roomPresence as never, realtimeEvents as never),
    prisma,
    realtimeEvents,
  };
}

describe('FriendsService realtime event protocol', () => {
  it('uses a dedicated request-received event for notifications', async () => {
    const { service, prisma, realtimeEvents } = makeService();
    prisma.user.findUnique.mockResolvedValue({ id: 'friend-b', email: 'b@example.test', displayName: 'B', avatarUrl: null, status: 'ONLINE' });
    prisma.friend.findUnique.mockResolvedValue(null);
    prisma.friend.create.mockResolvedValue({ id: 'request-1' });

    await service.add('user-a', 'friend-b');

    expect(realtimeEvents.emitToUser).toHaveBeenCalledWith(
      'friend-b',
      'friend:request-received',
      { requestId: 'request-1' },
    );
  });

  it('does not use the legacy generic friend:update protocol', async () => {
    const { service, prisma, realtimeEvents } = makeService();
    prisma.friend.findFirst.mockResolvedValue({ userId: 'user-a' });
    prisma.friend.deleteMany.mockResolvedValue({ count: 1 });

    await service.decline('user-b', 'request-1');

    expect(realtimeEvents.emitToUser).toHaveBeenNthCalledWith(
      1, 'user-b', 'friend:request-rejected', { requestId: 'request-1' },
    );
    expect(realtimeEvents.emitToUser).toHaveBeenNthCalledWith(
      2, 'user-a', 'friend:request-rejected', { requestId: 'request-1' },
    );
  });
});
