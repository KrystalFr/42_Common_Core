import { Module } from '@nestjs/common';
import { RealtimeEventsModule } from '../realtime/realtime-events.module.js';
import { LedgerController } from './ledger.controller.js';
import { LedgerService } from './ledger.service.js';

@Module({
  imports: [RealtimeEventsModule],
  controllers: [LedgerController],
  providers: [LedgerService],
  exports: [LedgerService],
})
export class LedgerModule {}
