import { Module } from '@nestjs/common';
import { RoundsController } from './rounds.controller.js';
import { RoundsService } from './rounds.service.js';
import { VideoDurationService } from './video-duration.service.js';
import { PrismaModule } from '../prisma/prisma.module.js';

@Module({
  imports: [PrismaModule],
  controllers: [RoundsController],
  providers: [RoundsService, VideoDurationService],
  exports: [RoundsService, VideoDurationService],
})
export class RoundsModule {}
