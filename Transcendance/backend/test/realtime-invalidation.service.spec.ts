import { jest } from '@jest/globals';
import { RealtimeInvalidationService } from '../src/realtime/realtime-invalidation.service.js';

describe('RealtimeInvalidationService', () => {
  it('deduplicates affected users and emits the canonical invalidation events', () => {
    const gateway = {
      invalidateStats: jest.fn(),
      invalidateHistory: jest.fn(),
      invalidateLeaderboard: jest.fn(),
    };
    const service = new RealtimeInvalidationService(gateway as never);

    service.afterRoundResolution({ userIds: ['user-a', 'user-a', 'user-b'] });

    expect(gateway.invalidateStats).toHaveBeenCalledTimes(2);
    expect(gateway.invalidateHistory).toHaveBeenCalledTimes(2);
    expect(gateway.invalidateStats).toHaveBeenNthCalledWith(1, 'user-a');
    expect(gateway.invalidateStats).toHaveBeenNthCalledWith(2, 'user-b');
    expect(gateway.invalidateLeaderboard).toHaveBeenCalledTimes(1);
  });

  it('does not invalidate the leaderboard when no profile was affected', () => {
    const gateway = {
      invalidateStats: jest.fn(),
      invalidateHistory: jest.fn(),
      invalidateLeaderboard: jest.fn(),
    };
    const service = new RealtimeInvalidationService(gateway as never);

    service.afterRoundResolution({ userIds: [] });

    expect(gateway.invalidateStats).not.toHaveBeenCalled();
    expect(gateway.invalidateHistory).not.toHaveBeenCalled();
    expect(gateway.invalidateLeaderboard).not.toHaveBeenCalled();
  });
});
