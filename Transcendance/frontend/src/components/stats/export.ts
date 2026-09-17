import { jsPDF } from 'jspdf';
import type { RoomStats, StatsDashboard } from '../../api/client';

type Cell = string | number | boolean | null | undefined;

type CsvRow = Record<string, Cell>;

const PDF = {
  margin: 42,
  width: 595.28,
  height: 841.89,
  text: [31, 41, 55] as const,
  muted: [100, 116, 139] as const,
  accent: [14, 116, 144] as const,
  accentLight: [224, 242, 254] as const,
  line: [226, 232, 240] as const,
  soft: [248, 250, 252] as const,
};

const dateFormatter = new Intl.DateTimeFormat('fr-FR', {
  dateStyle: 'medium',
  timeStyle: 'short',
});

const numberFormatter = new Intl.NumberFormat('fr-FR', { maximumFractionDigits: 2 });
const percentFormatter = new Intl.NumberFormat('fr-FR', { style: 'percent', maximumFractionDigits: 1 });

function text(value: Cell): string {
  if (value === null || value === undefined) return '';
  return String(value);
}

function date(value: Cell): string {
  if (!value) return '';
  const parsed = new Date(String(value));
  return Number.isNaN(parsed.getTime()) ? text(value) : dateFormatter.format(parsed);
}

function number(value: Cell): string {
  return value === null || value === undefined || value === '' ? '' : numberFormatter.format(Number(value));
}

function percent(value: Cell): string {
  return value === null || value === undefined || value === '' ? '' : percentFormatter.format(Number(value));
}

function downloadBlob(blob: Blob, filename: string) {
  const url = URL.createObjectURL(blob);
  const link = document.createElement('a');
  link.href = url;
  link.download = filename;
  document.body.appendChild(link);
  link.click();
  link.remove();
  window.setTimeout(() => URL.revokeObjectURL(url), 0);
}

function csvValue(value: Cell): string {
  const raw = text(value).replace(/\r?\n/g, ' ');
  return /[;"\n\r]/.test(raw) ? `"${raw.replace(/"/g, '""')}"` : raw;
}

function downloadCsv(rows: CsvRow[], filename: string) {
  const columns = [...new Set(rows.flatMap((row) => Object.keys(row)))];
  const content = [
    columns.map(csvValue).join(';'),
    ...rows.map((row) => columns.map((column) => csvValue(row[column])).join(';')),
  ].join('\r\n');

  // UTF-8 BOM + semicolon delimiter: Excel opens French-localized CSVs cleanly.
  downloadBlob(new Blob([`\uFEFF${content}`], { type: 'text/csv;charset=utf-8' }), filename);
}

function profileName(dashboard: StatsDashboard): string {
  return dashboard.user.displayName || dashboard.user.email || 'joueur';
}

function safeFilename(value: string): string {
  return value
    .normalize('NFD')
    .replace(/[\u0300-\u036f]/g, '')
    .replace(/[^a-zA-Z0-9_-]+/g, '-')
    .replace(/^-+|-+$/g, '')
    .toLowerCase() || 'export';
}

/**
 * Export personnel : orienté « analyse de mon activité ».
 * Les lignes sont volontairement normalisées par dataset pour rester faciles
 * à filtrer, croiser et grapher dans Excel / LibreOffice / Google Sheets.
 */
function personalCsvRows(dashboard: StatsDashboard): CsvRow[] {
  const rows: CsvRow[] = [];
  const user = dashboard.user;
  const common = { user_id: user.id, user_name: user.displayName ?? '', user_email: user.email };

  rows.push({
    dataset: 'profile', category: 'identity', ...common,
    status: user.status,
    profile_created_at: user.createdAt,
  });

  rows.push({
    dataset: 'summary', category: 'performance', ...common,
    period_key: dashboard.period.key,
    period_label: dashboard.period.label,
    period_from: dashboard.period.from ?? '',
    period_to: dashboard.period.to ?? '',
    current_balance: dashboard.currentBalance,
    recovery_available: dashboard.recoveryAvailable,
    rounds_played: dashboard.roundsPlayed,
    victories: dashboard.victories,
    defeats: dashboard.defeats,
    win_rate: dashboard.winRate,
    total_stake: dashboard.totalStake,
    total_payout: dashboard.totalPayout,
    total_won: dashboard.totalWon,
    total_lost: dashboard.totalLost,
    net_gain: dashboard.netGain,
    best_gain: dashboard.bestGain,
    worst_loss: dashboard.worstLoss,
  });

  dashboard.accuracy.byCategory.forEach((item) => {
    rows.push({
      dataset: 'accuracy_by_category', category: item.category, ...common,
      total_answers: item.total,
      correct_answers: item.correct,
      accuracy: item.accuracy,
    });
  });

  dashboard.balanceHistory.forEach((point) => {
    rows.push({
      dataset: 'balance_transactions', category: point.type, ...common,
      record_id: point.id,
      date: point.date,
      amount: point.amount,
      balance_after: point.balanceAfter,
      reference_id: point.referenceId ?? '',
      is_baseline: point.isBaseline ?? false,
    });
  });

  dashboard.roundBalanceHistory.forEach((point) => {
    rows.push({
      dataset: 'round_balance', category: 'round', ...common,
      round: point.round,
      round_id: point.id,
      date: point.date,
      balance_after: point.balanceAfter,
    });
  });

  (user.achievements ?? []).forEach((achievement) => {
    rows.push({
      dataset: 'achievements', category: achievement.achievementKey, ...common,
      achievement_key: achievement.achievementKey,
      unlocked_at: achievement.unlockedAt,
    });
  });

  return rows;
}

/**
 * Export de partie : orienté « reconstituer et analyser une partie ».
 * On conserve séparément joueurs, évolution financière, mises et votes afin
 * qu'un tableur puisse agréger chaque dataset sans ambiguïté.
 */
function roomCsvRows(stats: RoomStats): CsvRow[] {
  const rows: CsvRow[] = [];
  const room = stats.room;

  rows.push({
    dataset: 'game', category: 'room',
    room_id: room.id,
    room_name: room.name,
    room_status: room.status,
    room_created_at: room.createdAt,
    game_status: room.game?.status ?? '',
    current_round_index: room.game?.currentRoundIndex ?? '',
    game_started_at: room.game?.startedAt ?? '',
    game_finished_at: room.game?.finishedAt ?? '',
    round_count: stats.roundCount,
  });

  stats.players.forEach((player) => {
    rows.push({
      dataset: 'players', category: 'player',
      room_id: room.id,
      user_id: player.user.id,
      player_name: player.user.displayName ?? '',
      player_email: player.user.email,
      player_status: player.user.status,
      joined_profile_at: player.user.createdAt,
      initial_balance: player.initialBalance ?? '',
      rounds_played: player.roundsPlayed,
      victories: player.victories,
      win_rate: player.winRate,
      total_stake: player.totalStake,
      total_won: player.totalWon,
      total_lost: player.totalLost,
      net_gain: player.netGain,
      accuracy: player.accuracy,
      accuracy_answered: player.accuracyAnswered,
    });
  });

  stats.evolution.forEach((point) => {
    const { round, roundId, status, date: pointDate, ...balances } = point;
    Object.entries(balances).forEach(([userId, balance]) => {
      rows.push({
        dataset: 'balance_evolution', category: 'round_balance',
        room_id: room.id,
        round,
        round_id: roundId,
        status,
        date: pointDate,
        user_id: userId,
        balance_after: balance,
      });
    });
  });

  stats.roundDetails.forEach((round) => {
    round.bets.forEach((bet) => {
      rows.push({
        dataset: 'bets', category: 'bet',
        room_id: room.id,
        round: round.round,
        round_id: round.roundId,
        round_status: round.status,
        date: round.date,
        user_id: bet.userId,
        player_name: bet.player,
        stake: bet.stake,
        answer: bet.answer ?? '',
        result: bet.result ?? '',
        payout: bet.payout ?? '',
        variation: bet.variation ?? '',
      });
    });

    round.clips.forEach((clip) => {
      rows.push({
        dataset: 'clip_votes', category: clip.category,
        room_id: room.id,
        round: round.round,
        round_id: round.roundId,
        round_status: round.status,
        date: round.date,
        user_id: stats.players.find((player) => (player.user.displayName || player.user.email) === clip.player)?.user.id ?? '',
        player_name: clip.player,
        claim_id: clip.claimId,
        claim_or_clip: clip.clip,
        vote: clip.vote,
        correct: clip.correct,
      });
    });
  });

  return rows;
}

function setFill(doc: jsPDF, rgb: readonly [number, number, number]) {
  doc.setFillColor(rgb[0], rgb[1], rgb[2]);
}

function setText(doc: jsPDF, rgb: readonly [number, number, number]) {
  doc.setTextColor(rgb[0], rgb[1], rgb[2]);
}

function addHeader(doc: jsPDF, title: string, subtitle: string): number {
  setFill(doc, PDF.accent);
  doc.roundedRect(PDF.margin, 36, PDF.width - PDF.margin * 2, 72, 12, 12, 'F');
  doc.setFont('helvetica', 'bold');
  doc.setFontSize(23);
  doc.setTextColor(255, 255, 255);
  doc.text(title, PDF.margin + 18, 66);
  doc.setFont('helvetica', 'normal');
  doc.setFontSize(10);
  doc.text(subtitle, PDF.margin + 18, 88);
  return 130;
}

function addFooter(doc: jsPDF) {
  const pageCount = doc.getNumberOfPages();
  for (let page = 1; page <= pageCount; page += 1) {
    doc.setPage(page);
    setText(doc, PDF.muted);
    doc.setFont('helvetica', 'normal');
    doc.setFontSize(8);
    doc.text(`FACTARENA · Export du ${date(new Date().toISOString())}`, PDF.margin, PDF.height - 24);
    doc.text(`Page ${page} / ${pageCount}`, PDF.width - PDF.margin, PDF.height - 24, { align: 'right' });
  }
}

function ensureSpace(doc: jsPDF, y: number, needed = 42): number {
  if (y + needed <= PDF.height - 42) return y;
  doc.addPage();
  return 48;
}

function sectionTitle(doc: jsPDF, title: string, y: number): number {
  y = ensureSpace(doc, y, 34);
  setFill(doc, PDF.accentLight);
  doc.roundedRect(PDF.margin, y, PDF.width - PDF.margin * 2, 28, 7, 7, 'F');
  setText(doc, PDF.accent);
  doc.setFont('helvetica', 'bold');
  doc.setFontSize(11);
  doc.text(title, PDF.margin + 12, y + 18);
  return y + 40;
}

function metricCards(doc: jsPDF, metrics: Array<[string, string]>, y: number): number {
  const gap = 9;
  const width = (PDF.width - PDF.margin * 2 - gap * 2) / 3;
  metrics.forEach(([label, value], index) => {
    const x = PDF.margin + index * (width + gap);
    setFill(doc, PDF.soft);
    doc.roundedRect(x, y, width, 58, 9, 9, 'F');
    setText(doc, PDF.muted);
    doc.setFont('helvetica', 'bold');
    doc.setFontSize(8);
    doc.text(label.toUpperCase(), x + 10, y + 17);
    setText(doc, PDF.text);
    doc.setFontSize(15);
    doc.text(value, x + 10, y + 39);
  });
  return y + 72;
}

function simpleTable(doc: jsPDF, headers: string[], rows: string[][], y: number, widths?: number[]): number {
  const available = PDF.width - PDF.margin * 2;
  const columnWidths = widths ?? headers.map(() => available / headers.length);
  const headerHeight = 24;
  const lineHeight = 15;

  const drawHeader = (at: number) => {
    setFill(doc, PDF.accent);
    doc.rect(PDF.margin, at, available, headerHeight, 'F');
    setText(doc, [255, 255, 255]);
    doc.setFont('helvetica', 'bold');
    doc.setFontSize(7.5);
    let x = PDF.margin;
    headers.forEach((header, index) => {
      doc.text(header, x + 6, at + 16);
      x += columnWidths[index];
    });
    return at + headerHeight;
  };

  y = ensureSpace(doc, y, headerHeight + lineHeight * 2);
  y = drawHeader(y);

  rows.forEach((row) => {
    const maxLines = Math.max(...row.map((value, index) => doc.splitTextToSize(value, Math.max(20, columnWidths[index] - 12)).length));
    const rowHeight = Math.max(lineHeight, Math.min(42, maxLines * lineHeight + 5));
    if (y + rowHeight > PDF.height - 42) {
      doc.addPage();
      y = 48;
      y = drawHeader(y);
    }
    setFill(doc, [255, 255, 255]);
    doc.rect(PDF.margin, y, available, rowHeight, 'F');
    doc.setDrawColor(PDF.line[0], PDF.line[1], PDF.line[2]);
    doc.rect(PDF.margin, y, available, rowHeight, 'S');
    setText(doc, PDF.text);
    doc.setFont('helvetica', 'normal');
    doc.setFontSize(7.5);
    let x = PDF.margin;
    row.forEach((value, index) => {
      const lines = doc.splitTextToSize(value, Math.max(20, columnWidths[index] - 12)).slice(0, 3);
      doc.text(lines, x + 6, y + 12, { lineHeightFactor: 1.15 });
      x += columnWidths[index];
    });
    y += rowHeight;
  });

  return y + 16;
}

function personalPdf(dashboard: StatsDashboard): jsPDF {
  const doc = new jsPDF({ unit: 'pt', format: 'a4' });
  let y = addHeader(doc, 'Mon bilan FACTARENA', `${profileName(dashboard)} · ${dashboard.period.label}`);

  y = metricCards(doc, [
    ['Solde actuel', `${number(dashboard.currentBalance)} cr`],
    ['Taux de victoire', percent(dashboard.winRate)],
    ['Gain net', `${dashboard.netGain >= 0 ? '+' : ''}${number(dashboard.netGain)} cr`],
  ], y);

  y = metricCards(doc, [
    ['Manches jouées', number(dashboard.roundsPlayed)],
    ['Mises engagées', `${number(dashboard.totalStake)} cr`],
    ['Gains / pertes', `+${number(dashboard.totalWon)} / -${number(dashboard.totalLost)} cr`],
  ], y);

  y = sectionTitle(doc, 'Profil et période analysée', y);
  y = simpleTable(doc, ['Champ', 'Valeur'], [
    ['Identifiant', dashboard.user.id],
    ['Nom affiché', dashboard.user.displayName ?? '—'],
    ['E-mail', dashboard.user.email],
    ['Statut', dashboard.user.status],
    ['Profil créé le', date(dashboard.user.createdAt)],
    ['Période', dashboard.period.label],
    ['Récupération disponible', dashboard.recoveryAvailable ? 'Oui' : 'Non'],
    ['Généré le', date(dashboard.generatedAt)],
  ], y, [150, PDF.width - PDF.margin * 2 - 150]);

  y = sectionTitle(doc, 'Précision par catégorie', y);
  y = simpleTable(doc, ['Catégorie', 'Réponses', 'Correctes', 'Précision'], dashboard.accuracy.byCategory.map((item) => [
    item.category, number(item.total), number(item.correct), percent(item.accuracy),
  ]), y, [250, 80, 80, 93]);

  y = sectionTitle(doc, 'Évolution du solde par manche', y);
  simpleTable(doc, ['Manche', 'Date', 'Solde après manche'], dashboard.roundBalanceHistory.map((point) => [
    number(point.round), date(point.date), `${number(point.balanceAfter)} cr`,
  ]), y, [70, 250, 183]);

  addFooter(doc);
  return doc;
}

function roomPdf(stats: RoomStats): jsPDF {
  const doc = new jsPDF({ unit: 'pt', format: 'a4' });
  const room = stats.room;
  let y = addHeader(doc, `Partie · ${room.name}`, `${stats.roundCount} manche(s) · export généré le ${date(stats.generatedAt)}`);

  y = metricCards(doc, [
    ['Manches', number(stats.roundCount)],
    ['Joueurs', number(stats.players.length)],
    ['État', room.status],
  ], y);

  y = sectionTitle(doc, 'Résumé de la partie', y);
  y = simpleTable(doc, ['Champ', 'Valeur'], [
    ['Salon', room.name],
    ['Identifiant du salon', room.id],
    ['Statut du salon', room.status],
    ['Créé le', date(room.createdAt)],
    ['Statut du jeu', room.game?.status ?? '—'],
    ['Jeu commencé le', date(room.game?.startedAt)],
    ['Jeu terminé le', date(room.game?.finishedAt)],
    ['Dernier index de manche', number(room.game?.currentRoundIndex)],
  ], y, [190, PDF.width - PDF.margin * 2 - 190]);

  y = sectionTitle(doc, 'Classement et performance des joueurs', y);
  y = simpleTable(doc, ['Joueur', 'Manches', 'Victoires', 'Win rate', 'Précision', 'Gain net'], stats.players.map((player) => [
    player.user.displayName || player.user.email,
    number(player.roundsPlayed), number(player.victories), percent(player.winRate), percent(player.accuracy),
    `${player.netGain >= 0 ? '+' : ''}${number(player.netGain)} cr`,
  ]), y, [145, 55, 60, 65, 65, 85]);

  y = sectionTitle(doc, 'Évolution financière', y);
  const evolutionRows: string[][] = [];
  stats.evolution.forEach((point) => {
    const round = text(point.round);
    const pointDate = date(point.date);
    const playerBalances = stats.players.map((player) => [
      player.user.displayName || player.user.email,
      point[player.user.id],
    ] as [string, Cell]);
    playerBalances.forEach(([player, balance]) => {
      evolutionRows.push([round, pointDate, player, `${number(balance)} cr`]);
    });
  });
  y = simpleTable(doc, ['Manche', 'Date', 'Joueur', 'Solde'], evolutionRows, y, [55, 145, 205, 70]);

  y = sectionTitle(doc, 'Détail des mises', y);
  const betRows = stats.roundDetails.flatMap((round) => round.bets.map((bet) => [
    number(round.round), bet.player, number(bet.stake), bet.answer ?? '—', bet.result ?? '—',
    bet.payout === null ? '—' : number(bet.payout),
    bet.variation === null ? '—' : `${bet.variation >= 0 ? '+' : ''}${number(bet.variation)}`,
  ]));
  y = simpleTable(doc, ['Manche', 'Joueur', 'Mise', 'Réponse', 'Résultat', 'Paiement', 'Variation'], betRows, y,
    [45, 120, 55, 60, 65, 65, 70]);

  y = sectionTitle(doc, 'Votes sur les clips', y);
  const clipRows = stats.roundDetails.flatMap((round) => round.clips.map((clip) => [
    number(round.round), clip.player, clip.category, clip.vote, clip.correct ? 'Correct' : 'Incorrect',
  ]));
  simpleTable(doc, ['Manche', 'Joueur', 'Catégorie', 'Vote', 'Verdict'], clipRows, y, [50, 145, 150, 70, 95]);

  addFooter(doc);
  return doc;
}

export function exportPersonalCsv(dashboard: StatsDashboard) {
  const filename = `factarena-profil-${safeFilename(profileName(dashboard))}-${safeFilename(dashboard.period.key)}.csv`;
  downloadCsv(personalCsvRows(dashboard), filename);
}

export function exportRoomCsv(stats: RoomStats) {
  const filename = `factarena-partie-${safeFilename(stats.room.name)}-${safeFilename(stats.room.id)}.csv`;
  downloadCsv(roomCsvRows(stats), filename);
}

export function exportPersonalPdf(dashboard: StatsDashboard) {
  const filename = `factarena-profil-${safeFilename(profileName(dashboard))}-${safeFilename(dashboard.period.key)}.pdf`;
  personalPdf(dashboard).save(filename);
}

export function exportRoomPdf(stats: RoomStats) {
  const filename = `factarena-partie-${safeFilename(stats.room.name)}-${safeFilename(stats.room.id)}.pdf`;
  roomPdf(stats).save(filename);
}
