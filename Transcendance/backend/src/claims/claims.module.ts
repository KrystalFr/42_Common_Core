import { Module } from '@nestjs/common';
import { ClaimsController } from './claims.controller.js';
import { ClaimsService } from './claims.service.js';
import { PrismaModule } from '../prisma/prisma.module.js';
import { RolesGuard } from '../auth/guards/roles.guard.js';
import { Reflector } from '@nestjs/core';

@Module({
  imports: [PrismaModule],
  controllers: [ClaimsController],
  providers: [ClaimsService, RolesGuard],
  exports: [ClaimsService],
})
export class ClaimsModule {}
