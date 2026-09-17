import { BadRequestException, ConflictException, Injectable, NotFoundException } from '@nestjs/common';
import { Prisma } from '@prisma/client';
import { PrismaService } from '../prisma/prisma.service.js';
import { RealtimeEvents } from '../realtime/realtime.events.js';
import { isRecoveryAvailable } from './recovery-rules.js';

@Injectable()
export class LedgerService {
  static readonly MONTANT_RATTRAPAGE = 200;

  constructor(
    private readonly prisma: PrismaService,
    private readonly realtimeEvents: RealtimeEvents,
  ) {}

  async getBalance(userId: string) {
    const user = await this.prisma.user.findUnique({
      where: { id: userId },
      select: {
        virtualBalance: true,
        bankruptEpisode: true,
        recoveredEpisode: true,
        recoveryEligibleEpisode: true,
      },
    });
    if (!user) throw new NotFoundException('Utilisateur introuvable');
    return {
      virtualBalance: user.virtualBalance,
      recoveryAvailable: isRecoveryAvailable(user),
    };
  }

  async getHistory(userId: string) {
    return this.prisma.transaction.findMany({ where: { userId }, orderBy: { createdAt: 'desc' } });
  }

  async debitForBet(tx: Prisma.TransactionClient, userId: string, stake: number, referenceId?: string) {
    this.assertPositiveAmount(stake);
    if (referenceId) {
      const existing = await tx.transaction.findUnique({ where: { referenceId } });
      if (existing) {
        this.assertSameOperation(existing, userId, 'BET', -stake);
        return existing;
      }
    }
    const before = await tx.user.findUnique({ where: { id: userId }, select: { virtualBalance: true, bankruptEpisode: true } });
    if (!before || before.virtualBalance < stake) throw new BadRequestException('Crédits virtuels insuffisants');
    const updated = await tx.user.updateMany({ where: { id: userId, virtualBalance: { gte: stake } }, data: { virtualBalance: { decrement: stake } } });
    if (updated.count !== 1) throw new BadRequestException('Crédits virtuels insuffisants');
    const user = await tx.user.findUniqueOrThrow({ where: { id: userId }, select: { virtualBalance: true, bankruptEpisode: true } });
    if (before.virtualBalance > 0 && user.virtualBalance === 0) {
      await tx.user.update({ where: { id: userId }, data: { bankruptEpisode: { increment: 1 }, recoveryEligibleEpisode: null } });
    }
    return tx.transaction.create({ data: { userId, type: 'BET', amount: -stake, balanceAfter: user.virtualBalance, referenceId } });
  }

  async creditPayout(tx: Prisma.TransactionClient, userId: string, amount: number, referenceId?: string) {
    this.assertPositiveAmount(amount);
    if (referenceId) {
      const existing = await tx.transaction.findUnique({ where: { referenceId } });
      if (existing) {
        this.assertSameOperation(existing, userId, 'PAYOUT', amount);
        return existing;
      }
    }
    const user = await tx.user.update({ where: { id: userId }, data: { virtualBalance: { increment: amount } }, select: { virtualBalance: true } });
    return tx.transaction.create({ data: { userId, type: 'PAYOUT', amount, balanceAfter: user.virtualBalance, referenceId } });
  }

  async debitPenalty(tx: Prisma.TransactionClient, userId: string, referenceId: string) {
    const existing = await tx.transaction.findUnique({ where: { referenceId } });
    if (existing) {
      this.assertSameOperation(existing, userId, 'PENALTY', existing.amount);
      return existing;
    }
    const before = await tx.user.findUnique({ where: { id: userId }, select: { virtualBalance: true } });
    if (!before) throw new NotFoundException('Utilisateur introuvable');
    const amount = Math.min(50, before.virtualBalance);
    if (amount === 0) {
      return tx.transaction.create({ data: { userId, type: 'PENALTY', amount: 0, balanceAfter: 0, referenceId } });
    }
    const updated = await tx.user.updateMany({ where: { id: userId, virtualBalance: { gte: amount } }, data: { virtualBalance: { decrement: amount } } });
    if (updated.count !== 1) throw new BadRequestException('Impossible d’appliquer la pénalité');
    const user = await tx.user.findUniqueOrThrow({ where: { id: userId }, select: { virtualBalance: true, bankruptEpisode: true } });
    if (user.virtualBalance === 0) {
      const nextEpisode = user.bankruptEpisode + 1;
      await tx.user.update({
        where: { id: userId },
        data: { bankruptEpisode: nextEpisode, recoveryEligibleEpisode: nextEpisode },
      });
    }
    return tx.transaction.create({ data: { userId, type: 'PENALTY', amount: -amount, balanceAfter: user.virtualBalance, referenceId } });
  }

  async refundBet(tx: Prisma.TransactionClient, userId: string, amount: number, referenceId: string) {
    this.assertPositiveAmount(amount);
    if (!referenceId?.trim()) throw new BadRequestException('Une référence de remboursement est obligatoire');
    const existing = await tx.transaction.findUnique({ where: { referenceId } });
    if (existing) {
      this.assertSameOperation(existing, userId, 'REFUND', amount);
      return existing;
    }

    const user = await tx.user.update({ where: { id: userId }, data: { virtualBalance: { increment: amount } }, select: { virtualBalance: true } });
    return tx.transaction.create({ data: { userId, type: 'REFUND', amount, balanceAfter: user.virtualBalance, referenceId } });
  }


  async reclamerRattrapage(userId: string) {
    const result = await this.prisma.$transaction(async (tx) => {
      const user = await tx.user.findUnique({
        where: { id: userId },
        select: {
          virtualBalance: true,
          bankruptEpisode: true,
          recoveredEpisode: true,
          recoveryEligibleEpisode: true,
        },
      });
      if (!user) throw new NotFoundException('Utilisateur introuvable');
      if (!isRecoveryAvailable(user)) {
        if (user.virtualBalance !== 0) {
          throw new BadRequestException('La récupération est réservée aux joueurs à 0 crédit');
        }
        if (user.bankruptEpisode <= user.recoveredEpisode) {
          throw new BadRequestException('Cette récupération a déjà été utilisée pour cet épisode');
        }
        throw new BadRequestException('La récupération sera disponible après le règlement de la manche');
      }

      const updated = await tx.user.updateMany({
        where: {
          id: userId,
          virtualBalance: 0,
          recoveredEpisode: user.recoveredEpisode,
          bankruptEpisode: user.bankruptEpisode,
          recoveryEligibleEpisode: user.bankruptEpisode,
        },
        data: {
          virtualBalance: { increment: LedgerService.MONTANT_RATTRAPAGE },
          recoveredEpisode: user.bankruptEpisode,
          recoveryEligibleEpisode: null,
        },
      });
      if (updated.count !== 1) throw new ConflictException('Cette récupération a déjà été réclamée');

      const after = LedgerService.MONTANT_RATTRAPAGE;
      await tx.transaction.create({
        data: {
          userId,
          type: 'BONUS',
          amount: LedgerService.MONTANT_RATTRAPAGE,
          balanceAfter: after,
          referenceId: `recovery:${userId}:${user.bankruptEpisode}`,
        },
      });
      return { virtualBalance: after, credite: LedgerService.MONTANT_RATTRAPAGE, episode: user.bankruptEpisode };
    });
    this.realtimeEvents.emitToUser(userId, 'balance:update', {
      userId,
      virtualBalance: result.virtualBalance,
      recoveryAvailable: false,
    });
    return result;
  }

  async markRecoveryEligible(tx: Prisma.TransactionClient, userId: string) {
    const user = await tx.user.findUnique({
      where: { id: userId },
      select: { virtualBalance: true, bankruptEpisode: true, recoveredEpisode: true, recoveryEligibleEpisode: true },
    });
    if (!user || user.virtualBalance !== 0 || user.bankruptEpisode <= user.recoveredEpisode) return false;
    if (user.recoveryEligibleEpisode === user.bankruptEpisode) return true;

    await tx.user.update({
      where: { id: userId },
      data: { recoveryEligibleEpisode: user.bankruptEpisode },
    });
    return true;
  }

  private assertPositiveAmount(amount: number) {
    if (!Number.isSafeInteger(amount) || amount <= 0) {
      throw new BadRequestException('Le montant du ledger doit être un entier positif');
    }
  }

  private assertSameOperation(
    existing: { userId: string; type: string; amount: number },
    userId: string,
    type: string,
    amount: number,
  ) {
    if (existing.userId !== userId || existing.type !== type || existing.amount !== amount) {
      throw new ConflictException('La référence du ledger est déjà utilisée par une autre opération');
    }
  }
}
