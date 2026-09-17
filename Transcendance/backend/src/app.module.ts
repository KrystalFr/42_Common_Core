import { Module } from '@nestjs/common';
import { PrismaModule } from './prisma/prisma.module.js';
import { HealthModule } from './health/health.module.js';
import { AuthModule } from './auth/auth.module.js';
import { RoomsModule } from './rooms/rooms.module.js';
import { RoundsModule } from './rounds/rounds.module.js';
import { LedgerModule } from './ledger/ledger.module.js';
import { UsersModule } from './users/users.module.js';
import { FriendsModule } from './friends/friends.module.js';
import { ChatModule } from './chat/chat.module.js';
import { RealtimeModule } from './realtime/realtime.module.js';
import { ClaimsModule } from './claims/claims.module.js';
import { AiModule } from './ai/ai.module.js';
import { StatsModule } from './stats/stats.module.js';

@Module({
  imports: [
    PrismaModule,
    HealthModule,
    AuthModule,
    RoomsModule,
    RoundsModule,
    LedgerModule,
    UsersModule,
    FriendsModule,
    ChatModule,
    RealtimeModule,
    ClaimsModule,
    AiModule,
    StatsModule,
  ],
})
export class AppModule {}
