import { Module } from '@nestjs/common';
import { PrismaModule } from '../prisma/prisma.module.js';
import { RoomPresenceService } from './room-presence.service.js';

@Module({ imports: [PrismaModule], providers: [RoomPresenceService], exports: [RoomPresenceService] })
export class RoomPresenceModule {}
