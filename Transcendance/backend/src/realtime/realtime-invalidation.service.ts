import { Injectable, Logger } from '@nestjs/common';
import { RealtimeGateway } from './realtime.gateway.js';

export interface RoundResolutionInvalidation {
  userIds: string[];
}

@Injectable()
export class RealtimeInvalidationService {
  private readonly logger = new Logger(RealtimeInvalidationService.name);

  constructor(private readonly gateway: RealtimeGateway) {}

  afterRoundResolution({ userIds }: RoundResolutionInvalidation) {
    const affectedUserIds = [...new Set(userIds.filter(Boolean))];
    for (const userId of affectedUserIds) {
      this.gateway.invalidateStats(userId);
      this.gateway.invalidateHistory(userId);
    }
    if (affectedUserIds.length > 0) {
      this.gateway.invalidateLeaderboard();
    }
    this.logger.debug(`Round resolution invalidated ${affectedUserIds.length} profile(s)`);
  }
}
