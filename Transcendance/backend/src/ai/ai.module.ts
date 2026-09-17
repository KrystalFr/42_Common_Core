import { Module } from '@nestjs/common';
import { HttpModule } from '@nestjs/axios';
import { PrismaModule } from '../prisma/prisma.module.js';
import { RateLimitService } from '../common/rate-limit.service.js';
import { AiController } from './ai.controller.js';
import { AiService } from './ai.service.js';

@Module({
  imports: [HttpModule, PrismaModule],
  controllers: [AiController],
  providers: [AiService, RateLimitService],
  exports: [AiService, RateLimitService],
})
export class AiModule {}
