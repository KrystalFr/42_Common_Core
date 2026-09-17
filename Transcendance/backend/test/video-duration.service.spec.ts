import { jest } from '@jest/globals';
import { VideoDurationService } from '../src/rounds/video-duration.service.js';

describe('VideoDurationService', () => {
  afterEach(() => jest.restoreAllMocks());

  it('reuses a persisted duration', async () => {
    const prisma = { claim: { findUnique: jest.fn().mockResolvedValue({ id: 'c1', mediaRef: 'https://example.test/video.webm', videoEndS: 42 }), updateMany: jest.fn() } };
    const service = new VideoDurationService(prisma as never);
    await expect(service.ensureDuration('c1')).resolves.toBe(42);
    expect(prisma.claim.updateMany).not.toHaveBeenCalled();
  });

  it('calculates and persists a missing YouTube duration on the server', async () => {
    const prisma = { claim: { findUnique: jest.fn().mockResolvedValue({ id: 'c1', mediaRef: 'https://www.youtube-nocookie.com/embed/abc', videoEndS: null }), updateMany: jest.fn().mockResolvedValue({ count: 1 }) } };
    jest.spyOn(global, 'fetch').mockResolvedValue({ ok: true, status: 200, text: async () => '"lengthSeconds":"87"' } as Response);
    const service = new VideoDurationService(prisma as never);
    await expect(service.ensureDuration('c1')).resolves.toBe(87);
    expect(prisma.claim.updateMany).toHaveBeenCalledWith(expect.objectContaining({ data: { videoEndS: 87 } }));
  });
});
