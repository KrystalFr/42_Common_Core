import { Controller, Get, Param, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../auth/guards/jwt-auth.guard.js';
import { CuidParamPipe } from '../common/pipes/cuid-param.pipe.js';
import { RoundsService } from './rounds.service.js';

@Controller('rounds')
@UseGuards(JwtAuthGuard)
export class RoundsController {
  constructor(private readonly roundsService: RoundsService) {}

  @Get('room/:roomId')
  findByRoom(@Param('roomId', CuidParamPipe) roomId: string) { return this.roundsService.findByRoom(roomId); }

  @Get(':id/history')
  history(@Param('id', CuidParamPipe) id: string) { return this.roundsService.findOne(id); }

  @Get(':id')
  findOne(@Param('id', CuidParamPipe) id: string) { return this.roundsService.findOne(id); }
}
