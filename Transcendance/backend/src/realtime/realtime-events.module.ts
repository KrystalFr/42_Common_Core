import { Module } from '@nestjs/common';
import { PrismaModule } from '../prisma/prisma.module.js';
import { RoomPresenceModule } from '../rooms/room-presence.module.js';
import { RealtimeEvents } from './realtime.events.js';

@Module({
  imports: [PrismaModule, RoomPresenceModule],
  providers: [RealtimeEvents],
  exports: [RealtimeEvents],
})
export class RealtimeEventsModule {}
