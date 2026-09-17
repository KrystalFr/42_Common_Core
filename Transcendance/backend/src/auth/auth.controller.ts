import {
  Body,
  Controller,
  Get,
  Post,
  Req,
  Res,
  UnauthorizedException,
  UseGuards,
} from '@nestjs/common';
import type { Response, Request } from 'express';
import { AuthService } from './auth.service.js';
import { LoginDto, RegisterDto } from './dto/auth.dto.js';
import { getFrontendCallbackUrl } from '../common/env.js';
import { FortyTwoAuthGuard } from './guards/fortytwo-auth.guard.js';
import { AuthRateLimitGuard } from './guards/auth-rate-limit.guard.js';

@Controller('auth')
export class AuthController {
  constructor(private readonly authService: AuthService) {}

  @Post('register')
  @UseGuards(AuthRateLimitGuard)
  register(@Body() dto: RegisterDto) {
    return this.authService.register(dto);
  }

  @Post('login')
  @UseGuards(AuthRateLimitGuard)
  async login(@Body() dto: LoginDto) {
    try {
      return await this.authService.login(dto);
    } catch (error) {

      if (error instanceof UnauthorizedException) {
        return { loginFailed: true, message: error.message };
      }
      throw error;
    }
  }

  @Get('42')
  @UseGuards(FortyTwoAuthGuard)
  authenticateWith42() {

  }

  @Get('42/callback')
  @UseGuards(AuthRateLimitGuard, FortyTwoAuthGuard)
  async authenticateWith42Callback(
    @Req() req: Request & { user: { accessToken: string; user: any } },
    @Res() res: Response,
  ) {
    const token = req.user?.accessToken;
    const redirectUrl = token ? getFrontendCallbackUrl(token) : null;

    if (redirectUrl) {
      return res.redirect(redirectUrl);
    }

    return res.json(req.user);
  }
}
