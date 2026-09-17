import { Controller, Get, Param, Query, Request, UseGuards } from '@nestjs/common';
import { JwtAuthGuard } from '../auth/guards/jwt-auth.guard.js';
import { CuidParamPipe } from '../common/pipes/cuid-param.pipe.js';
import { StatsService } from './stats.service.js';

@Controller('stats')
@UseGuards(JwtAuthGuard)
export class StatsController {
  constructor(private readonly statsService: StatsService) {}

  @Get('leaderboard')
  leaderboard() { return this.statsService.getLeaderboard(); }

  @Get('round-history')
  roundHistory() { return this.statsService.getRoundHistory(); }

  @Get('me')
  myStats(@Request() req: { user: { userId: string } }) { return this.statsService.getUserStats(req.user.userId); }

  @Get('me/dashboard')
  myDashboard(@Request() req: { user: { userId: string } }, @Query() query: { period?: string; from?: string; to?: string }) {
    return this.statsService.getUserDashboard(req.user.userId, query);
  }

  @Get('me/round-history')
  myRoundHistory(@Request() req: { user: { userId: string } }) { return this.statsService.getUserRoundHistory(req.user.userId); }

  @Get('compare')
  compare(@Request() req: { user: { userId: string } }, @Query('userIds') userIds = '') {
    return this.statsService.getComparisonStats(req.user.userId, userIds.split(',').map((id) => id.trim()));
  }

  @Get('rooms/:roomId')
  roomStats(@Param('roomId', CuidParamPipe) roomId: string, @Request() req: { user: { userId: string } }) { return this.statsService.getRoomStats(roomId, req.user.userId); }

  @Get('users/:userId')
  userStats(@Param('userId', CuidParamPipe) userId: string) { return this.statsService.getUserStats(userId); }

  @Get('users/:userId/round-history')
  userRoundHistory(@Param('userId', CuidParamPipe) userId: string) { return this.statsService.getUserRoundHistory(userId); }
}
