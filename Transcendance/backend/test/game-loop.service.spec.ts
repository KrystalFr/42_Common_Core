import { jest } from '@jest/globals';
import { GameLoopService } from '../src/realtime/game-loop.service.js';

describe('GameLoopService', () => {
  it('starts ready rooms and resolves expired rounds without a host', async () => {
    const prisma = {
      game: { findMany: jest.fn().mockResolvedValue([]) },
      round: {
        findMany: jest.fn().mockResolvedValue([{ id: 'round-1', roomId: 'room-1', gameId: 'game-1' }]),
      },
      roomMember: {
        count: jest.fn().mockResolvedValue(2),
        findMany: jest.fn().mockResolvedValue([{ userId: 'user-a' }, { userId: 'user-b' }]),
      },
    };
    const games = {
      cancelInterruptedGames: jest.fn().mockResolvedValue([]),
      startReadyGames: jest.fn().mockResolvedValue(['room-1']),
      expireWaitingRooms: jest.fn().mockResolvedValue(['expired-room']),
      resolveRound: jest.fn().mockResolvedValue({ truth: 'FAKE', total: 100, good: 40, userIds: ['user-a', 'user-b'] }),
      penalizeMissingPlayers: jest.fn().mockResolvedValue({ kicked: [], cancelledUserId: undefined }),
      advanceAfterReveal: jest.fn().mockResolvedValue(null),
    };
    const gateway = {
      publishRoomState: jest.fn().mockResolvedValue({}),
      publishRoundState: jest.fn().mockResolvedValue({}),
      publishRoundVerdict: jest.fn(),
      broadcastRoomListUpdate: jest.fn(),
      broadcastBalanceUpdate: jest.fn().mockResolvedValue(undefined),
      ejectRoom: jest.fn().mockResolvedValue(undefined),
      server: { to: jest.fn().mockReturnValue({ emit: jest.fn() }) },
    };
    const invalidations = {
      afterRoundResolution: jest.fn(),
    };
    const rooms = {
      destroyRoomIfEmpty: jest.fn().mockResolvedValue(false),
    };
    const service = new GameLoopService(prisma as never, games as never, gateway as never, rooms as never, invalidations as never);
    await (service as unknown as { tick: () => Promise<void> }).tick();
    expect(games.startReadyGames).toHaveBeenCalled();
    expect(games.resolveRound).toHaveBeenCalledWith('round-1');
    expect(gateway.publishRoomState).toHaveBeenCalledWith('room-1', 'game:started');
    expect(gateway.publishRoundVerdict).toHaveBeenCalledWith('room-1', 'round-1', 'FAKE', 100, 40);
    expect(invalidations.afterRoundResolution).toHaveBeenCalledWith({ userIds: ['user-a', 'user-b'] });
    expect(invalidations.afterRoundResolution.mock.invocationCallOrder[0]).toBeGreaterThan(games.resolveRound.mock.invocationCallOrder[0]);
    expect(gateway.server.to).toHaveBeenCalledWith('room:expired-room');
    expect(gateway.ejectRoom).toHaveBeenCalledWith('expired-room');
  });
});
