import { Module } from '@nestjs/common';
import { RoomPresenceModule } from './room-presence.module.js';
import { ChatModule } from '../chat/chat.module.js';
import { RoomsController } from './rooms.controller.js';
import { RoomPresenceService } from './room-presence.service.js';
import { RoomsService } from './rooms.service.js';
import { RealtimeEventsModule } from '../realtime/realtime-events.module.js';

@Module({
  imports: [ChatModule, RealtimeEventsModule, RoomPresenceModule],
  controllers: [RoomsController],
  providers: [RoomsService],
  exports: [RoomsService, RoomPresenceModule],
})
export class RoomsModule {}
