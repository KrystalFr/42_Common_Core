import { Module } from '@nestjs/common';
import { AuthModule } from '../auth/auth.module.js';
import { LedgerModule } from '../ledger/ledger.module.js';
import { RoomsModule } from '../rooms/rooms.module.js';
import { RoundsModule } from '../rounds/rounds.module.js';
import { ChatModule } from '../chat/chat.module.js';
import { AiModule } from '../ai/ai.module.js';
import { RealtimeGateway } from './realtime.gateway.js';
import { GameLoopService } from './game-loop.service.js';
import { GameService } from './game.service.js';
import { BotsService } from './bots.service.js';
import { UsersModule } from '../users/users.module.js';
import { StatsModule } from '../stats/stats.module.js';
import { RealtimeInvalidationService } from './realtime-invalidation.service.js';
import { RealtimeEventsModule } from './realtime-events.module.js';

@Module({
  imports: [UsersModule, StatsModule, AuthModule, LedgerModule, RoomsModule, RoundsModule, ChatModule, AiModule, RealtimeEventsModule],
  providers: [RealtimeGateway, RealtimeInvalidationService, GameLoopService, GameService, BotsService],
  exports: [RealtimeGateway, RealtimeInvalidationService],
})
export class RealtimeModule {}
