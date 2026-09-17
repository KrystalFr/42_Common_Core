import { Body, Controller, Get, Param, Post, Req, Res, UseGuards } from '@nestjs/common';
import type { Request, Response } from 'express';
import { JwtAuthGuard } from '../auth/guards/jwt-auth.guard.js';
import { CuidParamPipe } from '../common/pipes/cuid-param.pipe.js';
import { RolesGuard } from '../auth/guards/roles.guard.js';
import { Roles } from '../auth/decorators/roles.decorator.js';
import { AiService } from './ai.service.js';
import { ClaimVerdictRequestDto } from './dto/claim-verdict-request.dto.js';
import { LlmRequestDto } from './dto/llm-request.dto.js';
import { ModerationRequestDto } from './dto/moderation-request.dto.js';
import { RagRequestDto } from './dto/rag-request.dto.js';

@Controller('ai')
@UseGuards(JwtAuthGuard, RolesGuard)
export class AiController {
  constructor(private readonly aiService: AiService) {}

  @Get('health')
  health() {
    return this.aiService.health();
  }

  @Post('rag')
  @Roles('ADMIN', 'SEED', 'USER')
  askRag(@Body() dto: RagRequestDto, @Req() req: Request) {

    return this.aiService.askRag(dto.question, dto.limit ?? 4, this.userId(req), {
      topics: dto.liveTopics,
      claims: dto.liveClaims,
    });
  }

  @Post('llm')
  @Roles('ADMIN', 'SEED', 'USER')
  chat(@Body() dto: LlmRequestDto, @Req() req: Request) {
    return this.aiService.chat(dto.prompt, dto.messages, this.userId(req));
  }

  @Post('llm/stream')
  async streamChat(@Body() dto: LlmRequestDto, @Req() req: Request, @Res() res: Response) {
    const userId = (req as Request & { user: { userId: string } }).user.userId;
    res.setHeader('Content-Type', 'text/event-stream; charset=utf-8');
    res.setHeader('Cache-Control', 'no-cache, no-transform');
    res.setHeader('Connection', 'keep-alive');
    for await (const event of this.aiService.streamChat(dto.prompt, userId)) {
      res.write(`event: ${event.type}\ndata: ${JSON.stringify(event.data)}\n\n`);
    }
    res.end();
  }

  @Post('moderation')
  @Roles('ADMIN', 'SEED', 'USER')
  moderate(@Body() dto: ModerationRequestDto, @Req() req: Request) {
    return this.aiService.moderate(dto.message, dto.history, this.userId(req));
  }

  @Post('claims/:claimId/verdict')
  @Roles('ADMIN', 'SEED', 'USER')
  generateClaimVerdict(@Param('claimId', CuidParamPipe) claimId: string, @Body() dto: ClaimVerdictRequestDto) {
    return this.aiService.generateClaimVerdict(claimId, dto.question, dto.limit ?? 4);
  }

  @Get('claims/:claimId/verdict')
  @Roles('ADMIN', 'SEED', 'USER')
  getClaimVerdict(@Param('claimId', CuidParamPipe) claimId: string) {
    return this.aiService.getClaimVerdict(claimId);
  }

  private userId(req: Request) {
    return (req as Request & { user: { userId: string } }).user.userId;
  }
}
