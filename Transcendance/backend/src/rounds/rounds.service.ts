import { Injectable, NotFoundException } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service.js';

@Injectable()
export class RoundsService {
  constructor(private readonly prisma: PrismaService) {}

  async assertBelongsToRoom(roundId: string, roomId: string): Promise<void> {
    const round = await this.prisma.round.findFirst({ where: { id: roundId, roomId }, select: { id: true } });
    if (!round) throw new NotFoundException(`La manche ${roundId} est introuvable dans le salon ${roomId}`);
  }

  async findOne(id: string) {
    const round = await this.prisma.round.findUnique({ where: { id }, include: this.historyInclude() });
    if (!round) throw new NotFoundException(`La manche ${id} est introuvable`);
    return this.sanitize(round);
  }

  async findByRoom(roomId: string) {
    const rounds = await this.prisma.round.findMany({ where: { roomId }, orderBy: [{ createdAt: 'asc' }, { id: 'asc' }], include: this.historyInclude() });
    return rounds.map((round) => this.sanitize(round));
  }

  private historyInclude() {
    return {
      room: { select: { id: true, name: true, status: true, createdBy: true } },
      game: { select: { id: true, status: true, currentRoundIndex: true } },
      roundClaims: { orderBy: { orderIndex: 'asc' as const }, include: { claim: true } },
      bets: { orderBy: [{ createdAt: 'asc' as const }, { id: 'asc' as const }], include: { user: { select: { id: true, email: true, displayName: true, avatarUrl: true, status: true } } } },
      clipVotes: true,
    };
  }

  private sanitize(round: any) {
    const revealed = ['REVEALED', 'FINISHED'].includes(String(round.status));
    const { bets, clipVotes: _clipVotes, ...safeRound } = round;
    return {
      ...safeRound,
      ...(revealed ? { bets } : {}),
      roundClaims: (round.roundClaims ?? []).map((rc: any) => {
        const { truthLabel, ...safeClaim } = rc.claim;
        return { id: rc.id, roundId: rc.roundId, claimId: rc.claimId, orderIndex: rc.orderIndex, claim: revealed ? rc.claim : safeClaim };
      }),
    };
  }
}
