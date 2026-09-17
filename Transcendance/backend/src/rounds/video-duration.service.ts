import { BadRequestException, Injectable, Logger } from '@nestjs/common';
import { execFile } from 'node:child_process';
import { promisify } from 'node:util';
import { URL } from 'node:url';
import { PrismaService } from '../prisma/prisma.service.js';

const execFileAsync = promisify(execFile);

@Injectable()
export class VideoDurationService {
  private readonly logger = new Logger(VideoDurationService.name);

  constructor(private readonly prisma: PrismaService) {}

  async ensureDuration(claimId: string): Promise<number> {
    const claim = await this.prisma.claim.findUnique({ where: { id: claimId }, select: { id: true, mediaRef: true, videoEndS: true } });
    if (!claim) throw new BadRequestException('Vidéo introuvable');
    if (Number.isInteger(claim.videoEndS) && (claim.videoEndS as number) > 0) return claim.videoEndS as number;
    if (!claim.mediaRef) throw new BadRequestException('Cette vidéo ne possède aucune source exploitable');

    const duration = await this.resolve(claim.mediaRef);
    if (!Number.isFinite(duration) || duration <= 0 || duration > 24 * 60 * 60) {
      throw new BadRequestException('Impossible de déterminer une durée vidéo valide');
    }
    const seconds = Math.ceil(duration);
    await this.prisma.claim.updateMany({
      where: { id: claimId, OR: [{ videoEndS: null }, { videoEndS: { lte: 0 } }] },
      data: { videoEndS: seconds },
    });
    return seconds;
  }

  private async resolve(mediaRef: string): Promise<number> {
    let url: URL;
    try {
      url = new URL(mediaRef);
    } catch {
      throw new BadRequestException('URL vidéo invalide');
    }
    if (!['http:', 'https:'].includes(url.protocol)) throw new BadRequestException('Protocole vidéo non supporté');

    const host = url.hostname.toLowerCase();
    if (host === 'youtube.com' || host.endsWith('.youtube.com') || host === 'youtu.be' || host.endsWith('.youtu.be') || host === 'youtube-nocookie.com' || host.endsWith('.youtube-nocookie.com')) {
      return this.resolveYouTube(url);
    }
    return this.resolveWithFfprobe(url.toString());
  }

  private async resolveYouTube(url: URL): Promise<number> {
    const response = await fetch(this.youtubeWatchUrl(url), {
      headers: { 'user-agent': 'FactArena/1.0 video-duration-resolver' },
      signal: AbortSignal.timeout(10_000),
    });
    if (!response.ok) throw new BadRequestException(`Impossible de récupérer les métadonnées YouTube (${response.status})`);
    const html = await response.text();
    const matches = [
      html.match(/"lengthSeconds":"(\d+)"/),
      html.match(/"lengthSeconds":(\d+)/),
    ];
    const seconds = matches.map((m) => Number(m?.[1])).find((value) => Number.isFinite(value) && value > 0);
    if (!seconds) {
      this.logger.warn(`Durée YouTube introuvable: ${url.toString()}`);
      throw new BadRequestException('Durée YouTube indisponible côté serveur');
    }
    return seconds;
  }

  private youtubeWatchUrl(url: URL): string {
    if (url.hostname === 'youtu.be' || url.hostname.endsWith('.youtu.be')) {
      return `https://www.youtube.com/watch?v=${encodeURIComponent(url.pathname.slice(1))}`;
    }
    if (url.pathname.startsWith('/embed/')) {
      return `https://www.youtube.com/watch?v=${encodeURIComponent(url.pathname.split('/')[2] ?? '')}`;
    }
    return `https://www.youtube.com/watch?v=${encodeURIComponent(url.searchParams.get('v') ?? '')}`;
  }

  private async resolveWithFfprobe(mediaRef: string): Promise<number> {
    try {
      const { stdout } = await execFileAsync('ffprobe', [
        '-v', 'error',
        '-show_entries', 'format=duration',
        '-of', 'default=noprint_wrappers=1:nokey=1',
        mediaRef,
      ], { timeout: 20_000, maxBuffer: 1024 * 1024 });
      const seconds = Number.parseFloat(stdout.trim());
      if (!Number.isFinite(seconds) || seconds <= 0) throw new Error('invalid duration');
      return seconds;
    } catch (error) {
      this.logger.warn(`ffprobe n'a pas pu déterminer la durée de ${mediaRef}: ${error instanceof Error ? error.message : String(error)}`);
      throw new BadRequestException('Durée vidéo indisponible côté serveur');
    }
  }
}
