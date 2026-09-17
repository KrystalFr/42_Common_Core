import { Global, Module, Provider } from '@nestjs/common';
import { JwtModule } from '@nestjs/jwt';
import { PassportModule } from '@nestjs/passport';
import { AuthController } from './auth.controller.js';
import { AuthService } from './auth.service.js';
import { PrismaModule } from '../prisma/prisma.module.js';
import { FortyTwoStrategy } from './strategies/fortytwo.strategy.js';
import { JwtAuthGuard } from './guards/jwt-auth.guard.js';
import { RolesGuard } from './guards/roles.guard.js';
import { FortyTwoAuthGuard } from './guards/fortytwo-auth.guard.js';
import { AuthRateLimitGuard } from './guards/auth-rate-limit.guard.js';
import { RateLimitService } from '../common/rate-limit.service.js';
import { getJwtSecret, isFortyTwoEnabled } from '../common/env.js';

const authProviders: Provider[] = [
  AuthService,
  JwtAuthGuard,
  RolesGuard,
  FortyTwoAuthGuard,
  AuthRateLimitGuard,
  RateLimitService,
];
if (isFortyTwoEnabled()) {
  authProviders.push(FortyTwoStrategy);
}


@Global()
@Module({
  imports: [
    PrismaModule,
    PassportModule.register({ session: false }),
    JwtModule.register({
      secret: getJwtSecret(),
      signOptions: { expiresIn: '1h' },
    }),
  ],
  controllers: [AuthController],
  providers: authProviders,
  exports: [JwtModule, JwtAuthGuard, RolesGuard, FortyTwoAuthGuard, AuthRateLimitGuard],
})
export class AuthModule {}
