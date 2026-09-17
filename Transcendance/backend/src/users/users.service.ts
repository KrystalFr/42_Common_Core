import {
  BadRequestException,
  Injectable,
  NotFoundException,
  UnauthorizedException,
} from '@nestjs/common';
import { Prisma } from '@prisma/client';
import * as bcrypt from 'bcryptjs';
import { PrismaService } from '../prisma/prisma.service.js';
import { ChangePasswordDto } from './dto/change-password.dto.js';
import { UpdateProfileDto } from './dto/update-profile.dto.js';
import { createReadStream, existsSync, readdirSync, statSync } from 'node:fs';
import { readFile, rename, unlink } from 'node:fs/promises';
import { randomUUID } from 'node:crypto';
import { join, resolve } from 'node:path';
import { StreamableFile } from '@nestjs/common';
import { RealtimeEvents } from '../realtime/realtime.events.js';

type AvatarFormat = 'png' | 'jpg' | 'webp';
type AvatarInfo = { format: AvatarFormat; width: number; height: number };

const MAX_AVATAR_WIDTH = 2048;
const MAX_AVATAR_HEIGHT = 2048;
const MAX_AVATAR_PIXELS = 4_000_000;
const TEMP_FILE_MAX_AGE_MS = 60 * 60_000;

@Injectable()
export class UsersService {
  constructor(
    private readonly prisma: PrismaService,
    private readonly realtimeEvents: RealtimeEvents,
  ) {}

  async findById(id: string) {
    const user = await this.prisma.user.findUnique({
      where: { id },
      select: {
        id: true,
        email: true,
        displayName: true,
        avatarUrl: true,
        status: true,
        role: true,
        activeRoomId: true,
        createdAt: true,
        updatedAt: true,
        achievements: { select: { achievementKey: true, unlockedAt: true }, orderBy: { unlockedAt: 'asc' } },
      },
    });

    if (!user) {
      throw new NotFoundException(`Utilisateur ${id} introuvable`);
    }

    return user;
  }


  async rechercher(demandeurId: string, texte: string) {
    const requete = texte.trim();
    if (requete.length < 2) return [];

    return this.prisma.user.findMany({
      where: {
        id: { not: demandeurId },
        role: { not: 'BOT' },
        OR: [
          { email: { contains: requete, mode: 'insensitive' } },
          { displayName: { contains: requete, mode: 'insensitive' } },
        ],
      },
      select: { id: true, email: true, displayName: true, avatarUrl: true, status: true },
      take: 10,
      orderBy: { email: 'asc' },
    });
  }

  async updateProfile(userId: string, dto: UpdateProfileDto) {
    const updateData: { displayName?: string; avatarUrl?: string } = {};

    if (dto.displayName !== undefined) {
      updateData.displayName = dto.displayName;
    }
    if (dto.avatarUrl !== undefined) {
      updateData.avatarUrl = dto.avatarUrl;
    }

    if (Object.keys(updateData).length === 0) {
      throw new BadRequestException('Aucun champ de profil fourni');
    }

    try {
      const user = await this.prisma.user.update({
        where: { id: userId },
        data: updateData,
        select: {
          id: true,
          email: true,
          displayName: true,
          avatarUrl: true,
          status: true,
          createdAt: true,
          updatedAt: true,
          achievements: { select: { achievementKey: true, unlockedAt: true }, orderBy: { unlockedAt: 'asc' } },
        },
      });
      await this.broadcastProfileUpdate(user);
      return user;
    } catch (error: unknown) {
      if (error instanceof Prisma.PrismaClientKnownRequestError && error.code === 'P2002') {
        throw new BadRequestException('Pseudo déjà utilisé par un autre joueur.');
      }
      throw error;
    }
  }

  async changePassword(userId: string, dto: ChangePasswordDto) {
    const user = await this.prisma.user.findUnique({
      where: { id: userId },
      select: { passwordHash: true },
    });

    if (!user) {
      throw new NotFoundException('Utilisateur introuvable');
    }

    if (user.passwordHash) {
      if (!dto.currentPassword) {
        throw new BadRequestException('Le mot de passe actuel est obligatoire');
      }

      const currentMatches = await bcrypt.compare(dto.currentPassword, user.passwordHash);
      if (!currentMatches) {
        throw new UnauthorizedException('Le mot de passe actuel est invalide');
      }
    }

    const passwordHash = await bcrypt.hash(dto.newPassword, 10);
    await this.prisma.user.update({
      where: { id: userId },
      data: { passwordHash },
    });

    return { success: true };
  }

  private async broadcastProfileUpdate(user: { id: string; email: string; displayName: string | null; avatarUrl: string | null; status: string; createdAt: Date; updatedAt: Date; achievements: Array<{ achievementKey: string; unlockedAt: Date }> }) {
    const relations = await this.prisma.friend.findMany({
      where: { status: 'ACCEPTED', OR: [{ userId: user.id }, { friendId: user.id }] },
      select: { userId: true, friendId: true },
    });
    const recipients = new Set([user.id, ...relations.map((relation) => relation.userId === user.id ? relation.friendId : relation.userId)]);
    for (const recipientId of recipients) this.realtimeEvents.emitToUser(recipientId, 'profile:update', user);
  }

  async setAvatar(userId: string, filename: string) {
    if (!this.isAvatarFilename(filename)) throw new BadRequestException('Fichier avatar invalide');
    const user = await this.prisma.user.findUnique({ where: { id: userId }, select: { avatarUrl: true } });
    if (!user) throw new NotFoundException('Utilisateur introuvable');

    const avatarUrl = `/api/users/avatars/${filename}`;
    const previous = user.avatarUrl?.match(/^\/api\/users\/avatars\/([a-zA-Z0-9._-]+)$/)?.[1];
    await this.prisma.user.update({ where: { id: userId }, data: { avatarUrl }, select: { id: true } });
    if (previous) {
      const previousPath = join(this.avatarDirectory(), previous);
      if (existsSync(previousPath)) await this.removeFile(previousPath);
    }
    const updatedUser = await this.findById(userId);
    this.broadcastProfileUpdate(updatedUser);
    return updatedUser;
  }

  async setAvatarFromUpload(userId: string, temporaryPath: string) {
    let finalPath: string | undefined;
    try {
      const avatar = await this.inspectAvatar(temporaryPath);
      const filename = `${randomUUID()}.${avatar.format}`;
      finalPath = join(this.avatarDirectory(), filename);
      await rename(temporaryPath, finalPath);
      return await this.setAvatar(userId, filename);
    } catch (error) {
      await this.removeFile(temporaryPath);
      if (finalPath) await this.removeFile(finalPath);
      if (error instanceof BadRequestException || error instanceof NotFoundException) throw error;
      throw error;
    }
  }

  async cleanupOrphanedAvatars() {
    if (!existsSync(this.avatarDirectory())) return { removed: 0 };
    const users = await this.prisma.user.findMany({
      select: { avatarUrl: true },
    });
    const referenced = new Set(
      users
        .map(({ avatarUrl }) => avatarUrl?.match(/^\/api\/users\/avatars\/([a-zA-Z0-9_-]+\.(?:png|jpe?g|webp))$/i)?.[1])
        .filter((filename): filename is string => Boolean(filename)),
    );
    let removed = 0;
    for (const filename of readdirSync(this.avatarDirectory())) {
      const filePath = join(this.avatarDirectory(), filename);
      if (filename.endsWith('.upload')) {
        if (Date.now() - statSync(filePath).mtimeMs > TEMP_FILE_MAX_AGE_MS) {
          await this.removeFile(filePath);
          removed += 1;
        }
        continue;
      }
      if (!this.isAvatarFilename(filename) || referenced.has(filename)) continue;
      await this.removeFile(filePath);
      removed += 1;
    }
    return { removed };
  }

  getAvatar(filename: string) {
    if (!/^[a-zA-Z0-9_-]+\.(png|jpe?g|webp)$/i.test(filename)) {
      throw new NotFoundException('Avatar introuvable');
    }
    const filePath = join(this.avatarDirectory(), filename);
    if (!existsSync(filePath)) throw new NotFoundException('Avatar introuvable');
    return new StreamableFile(createReadStream(filePath));
  }

  avatarDirectory() {
    return resolve(process.env.UPLOAD_DIR || join(process.cwd(), 'uploads', 'avatars'));
  }

  private async inspectAvatar(filePath: string): Promise<AvatarInfo> {
    const bytes = await readFile(filePath);
    const avatar = this.detectAvatar(bytes);
    if (!avatar) throw new BadRequestException('Le contenu du fichier ne correspond pas à son extension : seuls les vrais PNG, JPEG et WebP sont acceptés');
    if (avatar.width < 1 || avatar.height < 1 || avatar.width > MAX_AVATAR_WIDTH || avatar.height > MAX_AVATAR_HEIGHT || avatar.width * avatar.height > MAX_AVATAR_PIXELS) {
      throw new BadRequestException('Les dimensions de l’avatar sont trop grandes');
    }
    return avatar;
  }

  private detectAvatar(bytes: Buffer): AvatarInfo | null {
    if (bytes.length >= 24 && bytes.subarray(0, 8).equals(Buffer.from([137, 80, 78, 71, 13, 10, 26, 10])) && bytes.toString('ascii', 12, 16) === 'IHDR') {
      return { format: 'png', width: bytes.readUInt32BE(16), height: bytes.readUInt32BE(20) };
    }
    if (bytes.length >= 2 && bytes[0] === 0xff && bytes[1] === 0xd8) {
      return this.detectJpeg(bytes);
    }
    if (bytes.length >= 30
      && bytes.toString('ascii', 0, 4) === 'RIFF'
      && bytes.toString('ascii', 8, 12) === 'WEBP') {
      return this.detectWebp(bytes);
    }
    return null;
  }

  // Les trois variantes du conteneur WebP stockent les dimensions differemment.
  private detectWebp(bytes: Buffer): AvatarInfo | null {
    const chunk = bytes.toString('ascii', 12, 16);

    // VP8 : image lossy. Les dimensions suivent le start code 0x9d 0x01 0x2a.
    if (chunk === 'VP8 ') {
      if (bytes.length < 30) return null;
      if (bytes[23] !== 0x9d || bytes[24] !== 0x01 || bytes[25] !== 0x2a) return null;
      return {
        format: 'webp',
        width: bytes.readUInt16LE(26) & 0x3fff,
        height: bytes.readUInt16LE(28) & 0x3fff,
      };
    }

    // VP8L : image lossless. 14 bits par dimension, encodes moins un.
    if (chunk === 'VP8L') {
      if (bytes.length < 25 || bytes[20] !== 0x2f) return null;
      const bits = bytes.readUInt32LE(21);
      return {
        format: 'webp',
        width: (bits & 0x3fff) + 1,
        height: ((bits >>> 14) & 0x3fff) + 1,
      };
    }

    // VP8X : conteneur etendu (animation, alpha). Dimensions du canevas sur 24 bits.
    if (chunk === 'VP8X') {
      if (bytes.length < 30) return null;
      return {
        format: 'webp',
        width: bytes.readUIntLE(24, 3) + 1,
        height: bytes.readUIntLE(27, 3) + 1,
      };
    }

    return null;
  }

  private detectJpeg(bytes: Buffer): AvatarInfo | null {
    let offset = 2;
    while (offset + 9 < bytes.length) {
      if (bytes[offset] !== 0xff) { offset += 1; continue; }
      const marker = bytes[offset + 1];
      offset += 2;
      if (marker === 0xd8 || marker === 0xd9) continue;
      if (offset + 2 > bytes.length) return null;
      const length = bytes.readUInt16BE(offset);
      if (length < 2 || offset + length > bytes.length) return null;
      const isStartOfFrame = marker >= 0xc0 && marker <= 0xc3 || marker >= 0xc5 && marker <= 0xc7 || marker >= 0xc9 && marker <= 0xcb || marker >= 0xcd && marker <= 0xcf;
      if (isStartOfFrame && length >= 7) {
        return { format: 'jpg', height: bytes.readUInt16BE(offset + 3), width: bytes.readUInt16BE(offset + 5) };
      }
      offset += length;
    }
    return null;
  }

  private isAvatarFilename(filename: string): boolean {
    return /^[a-zA-Z0-9_-]+\.(png|jpe?g|webp)$/i.test(filename);
  }

  private async removeFile(filePath: string) {
    try { await unlink(filePath); } catch {  }
  }
}
