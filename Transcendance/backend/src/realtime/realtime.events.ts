import { Injectable } from '@nestjs/common';
import type { Server } from 'socket.io';
import { PrismaService } from '../prisma/prisma.service.js';
import { RoomPresenceService } from '../rooms/room-presence.service.js';

@Injectable()
export class RealtimeEvents {
  private server?: Server;

  constructor(
    private readonly prisma: PrismaService,
    private readonly presence: RoomPresenceService,
  ) {}

  attach(server: Server) {
    this.server = server;
  }

  emitToUser(userId: string, event: string, payload: unknown) {
    this.server?.to(this.userChannel(userId)).emit(event, payload);
  }

  emitToRoom(roomId: string, event: string, payload: unknown) {
    this.server?.to(this.roomChannel(roomId)).emit(event, payload);
  }

  emitAll(event: string, payload?: unknown) {
    this.server?.emit(event, payload);
  }

  async ejectUserFromRoom(roomId: string, userId: string) {
    for (const socket of this.server?.sockets.sockets.values() ?? []) {
      if (socket.data?.roomId !== roomId || socket.data?.userId !== userId) continue;
      await socket.leave(this.roomChannel(roomId));
      delete socket.data.roomId;
      await this.presence.leave(socket.id);
    }
    await this.syncUserPresence(userId);
  }

  async ejectRoom(roomId: string) {
    for (const socket of this.server?.sockets.sockets.values() ?? []) {
      if (socket.data?.roomId !== roomId) continue;
      await socket.leave(this.roomChannel(roomId));
      delete socket.data.roomId;
      await this.presence.leave(socket.id);
    }
    await this.presence.clearRoom(roomId);
    const userIds = new Set([...this.server?.sockets.sockets.values() ?? []].map((socket) => socket.data?.userId).filter((id): id is string => typeof id === 'string'));
    await Promise.all([...userIds].map((userId) => this.syncUserPresence(userId)));
  }

  async syncUserPresence(userId: string, forceNotify = false) {
    const currentRoomId = await this.presence.getCurrentRoomId(userId);
    const connected = [...this.server?.sockets.sockets.values() ?? []].some((socket) => socket.data?.userId === userId);
    const status = currentRoomId ? 'IN_GAME' : connected ? 'ONLINE' : 'OFFLINE';
    const updated = await this.prisma.user.updateMany({
      where: { id: userId, ...(forceNotify ? {} : { status: { not: status } }) },
      data: { status },
    });
    if (updated.count === 0 && !forceNotify) return;
    const relations = await this.prisma.friend.findMany({
      where: { status: 'ACCEPTED', OR: [{ userId }, { friendId: userId }] },
      select: { userId: true, friendId: true },
    });
    const payload = { userId, status, currentRoomId };
    for (const relation of relations) {
      const friendId = relation.userId === userId ? relation.friendId : relation.userId;
      this.emitToUser(friendId, 'presence:update', payload);
    }
  }

  roomChannel(id: string) {
    return `room:${id}`;
  }

  userChannel(id: string) {
    return `user:${id}`;
  }
}
