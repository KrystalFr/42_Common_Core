import { jest } from '@jest/globals';
import { NotFoundException } from '@nestjs/common';
import { RoundsService } from '../src/rounds/rounds.service.js';

describe('RoundsService room ownership', () => {
  it('accepts a round that belongs to the requested room', async () => {
    const prisma = { round: { findFirst: jest.fn().mockResolvedValue({ id: 'round-1' }) } };
    const service = new RoundsService(prisma as never);
    await expect(service.assertBelongsToRoom('round-1', 'room-1')).resolves.toBeUndefined();
  });

  it('rejects a round that belongs to another room', async () => {
    const prisma = { round: { findFirst: jest.fn().mockResolvedValue(null) } };
    const service = new RoundsService(prisma as never);
    await expect(service.assertBelongsToRoom('round-1', 'room-2')).rejects.toBeInstanceOf(NotFoundException);
  });
});
