import { Module } from '@nestjs/common';
import { PrismaModule } from '../prisma/prisma.module.js';
import { RoomsModule } from '../rooms/rooms.module.js';
import { FriendsController } from './friends.controller.js';
import { FriendsService } from './friends.service.js';
import { RealtimeEventsModule } from '../realtime/realtime-events.module.js';

@Module({
  imports: [PrismaModule, RoomsModule, RealtimeEventsModule],
  controllers: [FriendsController],
  providers: [FriendsService],
  exports: [FriendsService],
})
export class FriendsModule {}
