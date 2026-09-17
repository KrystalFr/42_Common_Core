import { Injectable } from '@nestjs/common';
import { PrismaService } from '../prisma/prisma.service.js';

const PRESENCE_TTL_MS = 45_000;

type PresenceEntry = {
  socketId: string;
  userId: string;
  roomId: string;
  lastSeenAt: Date;
};


@Injectable()
export class RoomPresenceService {
  constructor(private readonly prisma: PrismaService) {}

  private activeSince() {
    return new Date(Date.now() - PRESENCE_TTL_MS);
  }

  async join(socketId: string, userId: string, roomId: string) {
    await this.prune();
    const previous = await this.prisma.roomPresence.findUnique({ where: { socketId } });

    const activeMembers = await this.prisma.roomPresence.findMany({
      where: { roomId, lastSeenAt: { gt: this.activeSince() } },
      distinct: ['userId'],
      select: { userId: true },
    });
    const joueurDejaDansLaRoom = activeMembers.some((entry) => entry.userId === userId);


    await this.prisma.roomPresence.deleteMany({
      where: { roomId, userId, socketId: { not: socketId } },
    });
    await this.prisma.roomPresence.upsert({
      where: { socketId },
      update: { userId, roomId, lastSeenAt: new Date() },
      create: { socketId, userId, roomId },
    });

    return {
      previousRoomId: previous?.roomId,
      joinedRoom: !joueurDejaDansLaRoom,
    };
  }

  async touch(socketId: string) {
    await this.prisma.roomPresence.updateMany({
      where: { socketId },
      data: { lastSeenAt: new Date() },
    });
  }


  async rafraichir(entrees: Array<{ socketId: string; userId: string; roomId: string }>) {
    if (!entrees.length) return;
    const membres = await this.prisma.roomMember.findMany({
      where: {
        OR: entrees.map((e) => ({ roomId: e.roomId, userId: e.userId })),
      },
      select: { roomId: true, userId: true },
    });
    const autorises = new Set(membres.map((m) => `${m.roomId}:${m.userId}`));
    const maintenant = new Date();
    const refusees = entrees.filter((e) => !autorises.has(`${e.roomId}:${e.userId}`)).map((e) => e.socketId);
    if (refusees.length) {
      await this.prisma.roomPresence.deleteMany({ where: { socketId: { in: refusees } } });
    }
    await Promise.all(entrees
      .filter((e) => autorises.has(`${e.roomId}:${e.userId}`))
      .map((e) => this.prisma.roomPresence.upsert({
        where: { socketId: e.socketId },
        update: { userId: e.userId, roomId: e.roomId, lastSeenAt: maintenant },
        create: { socketId: e.socketId, userId: e.userId, roomId: e.roomId },
      })));
  }

  async leave(socketId: string) {
    const current = await this.prisma.roomPresence.findUnique({ where: { socketId } });
    if (!current) return null;


    await this.prisma.roomPresence.deleteMany({ where: { socketId } });
    const remaining = await this.prisma.roomPresence.count({
      where: { roomId: current.roomId, userId: current.userId, lastSeenAt: { gt: this.activeSince() } },
    });

    return { ...current, leftRoom: remaining === 0 };
  }

  async clearRoom(roomId: string) {
    const entries = await this.prisma.roomPresence.findMany({
      where: { roomId },
      select: { socketId: true, userId: true },
    });
    await this.prisma.roomPresence.deleteMany({ where: { roomId } });
    return entries;
  }

  async getMemberIds(roomId: string) {
    await this.prune();
    const entries = await this.prisma.roomPresence.findMany({
      where: { roomId, lastSeenAt: { gt: this.activeSince() } },
      distinct: ['userId'],
      select: { userId: true },
    });
    return entries.map((entry) => entry.userId);
  }

  async getCurrentRoomId(userId: string) {
    await this.prune();
    const entry = await this.prisma.roomPresence.findFirst({
      where: { userId, lastSeenAt: { gt: this.activeSince() } },
      orderBy: { lastSeenAt: 'desc' },
      select: { roomId: true },
    });
    return entry?.roomId ?? null;
  }

  async isMember(userId: string, roomId: string) {
    const [entry, member] = await Promise.all([
      this.prisma.roomPresence.findFirst({
        where: { userId, roomId, lastSeenAt: { gt: this.activeSince() } },
        select: { socketId: true },
      }),
      this.prisma.roomMember.findUnique({
        where: { roomId_userId: { roomId, userId } },
        select: { id: true },
      }),
    ]);
    return Boolean(entry && member);
  }

  private async prune() {
    const stale = await this.prisma.roomPresence.findMany({
      where: { lastSeenAt: { lte: this.activeSince() } },
      distinct: ['userId'],
      select: { userId: true },
    });
    if (stale.length === 0) return;

    await this.prisma.roomPresence.deleteMany({ where: { lastSeenAt: { lte: this.activeSince() } } });
    const stillActive = await this.prisma.roomPresence.findMany({
      where: { userId: { in: stale.map((entry) => entry.userId) }, lastSeenAt: { gt: this.activeSince() } },
      distinct: ['userId'],
      select: { userId: true },
    });
    const activeIds = new Set(stillActive.map((entry) => entry.userId));
    const offlineIds = stale.map((entry) => entry.userId).filter((id) => !activeIds.has(id));
    if (offlineIds.length > 0) {
      await this.prisma.user.updateMany({ where: { id: { in: offlineIds } }, data: { status: 'OFFLINE' } });
    }
  }
}
