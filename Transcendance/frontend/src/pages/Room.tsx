import { useEffect, useLayoutEffect, useMemo, useRef, useState } from 'react';
import { useNavigate, useOutletContext, useParams } from 'react-router';
import VideoPlayer from '../components/VideoPlayer';
import ResultsReveal from '../components/ResultsReveal';
import Chat from '../components/Chat';
import { useAuth } from '../auth/AuthContext';
import { useSocket } from '../hooks/useSocket';
import { api, solde, statistiquesRoom, chercherJoueurs, ajouterAmi, accepterDemande, demandesAmis, refuserDemande, type Ami, type RoomStats } from '../api/client';
import { RoomCreditsChart, RoomAccuracyChart, RoomCumulativeGainChart } from '../components/stats/Charts';
import { exportRoomCsv, exportRoomPdf } from '../components/stats/export';
import type { ChatMessage, ClipResult, LeaderboardEntry } from '../mocks/gameData';

const BET_OPTIONS = [100, 250, 500];

type LayoutRoomContext = {
  registerRoomLeave: (handler: (() => Promise<void>) | null) => void;
};

function formatCountdown(ms: number) {
  const seconds = Math.max(0, Math.ceil(ms / 1000));
  return `${Math.floor(seconds / 60)}:${String(seconds % 60).padStart(2, '0')}`;
}

function majMessageBot(
  messages: ChatMessage[],
  refFlux: { current: string | null },
  maj: (msg: ChatMessage) => ChatMessage,
): ChatMessage[] {
  const index = refFlux.current ? messages.findIndex((m) => m.id === refFlux.current) : -1;
  if (index === -1) {
    const nouveau = maj({
      id: `bot-${Date.now()}-${Math.random().toString(36).slice(2, 8)}`,
      type: 'bot',
      author: 'assistant IA',
      content: '',
      timestamp: Date.now(),
    });
    refFlux.current = nouveau.id;
    return [...messages, nouveau];
  }
  const copie = [...messages];
  copie[index] = maj(copie[index]);
  return copie;
}

export default function Room() {
  const { roomId } = useParams<{ roomId: string }>();
  const { utilisateur } = useAuth();
  const navigate = useNavigate();
  const { registerRoomLeave } = useOutletContext<LayoutRoomContext>();
  const { connecte, etat, erreur, emettre, demander, ecouter, deconnecterVolontairement, maintenantServeur } = useSocket(roomId ?? null);
  const [balance, setBalance] = useState(0);
  const [recoveryAvailable, setRecoveryAvailable] = useState(false);
  const [now, setNow] = useState(Date.now());
  const [stake, setStake] = useState<number | null>(null);
  const [customStake, setCustomStake] = useState('');
  const [stakeValidationError, setStakeValidationError] = useState<string | null>(null);
  const [amountConfirmed, setAmountConfirmed] = useState(false);
  const [answered, setAnswered] = useState<'FACT' | 'FAKE' | null>(null);
  const [roundTruth, setRoundTruth] = useState<'FACT' | 'FAKE' | null>(null);
  const [messages, setMessages] = useState<ChatMessage[]>([]);
  const [actionError, setActionError] = useState<string | null>(null);
  const [recoveryInProgress, setRecoveryInProgress] = useState(false);
  const [finalResults, setFinalResults] = useState<{ correctCount: number; clipResults: ClipResult[]; leaderboard: LeaderboardEntry[] } | null>(null);
  const [roomStats, setRoomStats] = useState<RoomStats | null>(null);
  const [roomStatsOpen, setRoomStatsOpen] = useState(false);
  const [roomStatsLoading, setRoomStatsLoading] = useState(false);
  const [roomStatsError, setRoomStatsError] = useState(false);
  const finalResultsRoomRef = useRef<string | null>(null);
  const membersRef = useRef<any[]>([]);
  const botStreamRef = useRef<string | null>(null);

  useLayoutEffect(() => {
    sessionStorage.setItem('factarena:room-loading-in-progress', '1');
    window.dispatchEvent(new Event('factarena:room-creation-state'));
  }, []);

  useEffect(() => {
    if (!etat?.room) return;
    sessionStorage.removeItem('factarena:room-loading-in-progress');
    if (sessionStorage.getItem('factarena:room-creation-in-progress') !== '1') {
      window.dispatchEvent(new Event('factarena:room-creation-state'));
      return;
    }
    sessionStorage.removeItem('factarena:room-creation-in-progress');
    window.dispatchEvent(new Event('factarena:room-creation-state'));
  }, [etat?.room]);

  useEffect(() => {
    const id = window.setInterval(() => setNow(maintenantServeur()), 250);
    return () => window.clearInterval(id);
  }, [maintenantServeur]);

  useEffect(() => {
    solde().then((r) => { setBalance(r.virtualBalance); setRecoveryAvailable(Boolean(r.recoveryAvailable)); }).catch(() => {});
  }, []);

  useEffect(() => {
    if (!roomId || !etat?.room?.id) return;
    let actif = true;
    setRoomStatsLoading(true);
    setRoomStatsError(false);
    statistiquesRoom(roomId).then((data) => { if (actif) setRoomStats(data); })
      .catch(() => { if (actif) setRoomStatsError(true); })
      .finally(() => { if (actif) setRoomStatsLoading(false); });
    return () => { actif = false; };
  }, [roomId, etat?.room?.id, etat?.round?.id, etat?.round?.status]);

  const round = etat?.round as any;
  const claim = round?.roundClaims?.[0]?.claim;
  const lobby = etat?.room?.status === 'WAITING';
  const game = etat?.room?.status === 'IN_PROGRESS';
  const recoveryVisible = recoveryAvailable;
  const lobbyRemaining = etat?.room?.lobbyExpiresAt ? new Date(etat.room.lobbyExpiresAt).getTime() - now : 0;
  const finishedAt = etat?.room?.game?.finishedAt ? new Date(etat.room.game.finishedAt).getTime() : null;
  const finishedRemaining = finishedAt ? Math.max(0, finishedAt + 60_000 - now) : 60_000;
  const transitionEndsAt = round?.transitionEndsAt as string | null | undefined;
  const roundRemaining = round?.status === 'REVEALED'
    ? transitionEndsAt ? new Date(transitionEndsAt).getTime() - now : 0
    : round?.locksAt ? new Date(round.locksAt).getTime() - now : 0;
  const members = (etat?.members ?? []) as any[];
  membersRef.current = members;
  const me = members.find((m) => m.id === utilisateur?.id);
  const gameRoundIndex = etat?.room?.game?.currentRoundIndex ?? 0;
  const myBet = (round?.bets ?? []).find((b: any) => b.userId === utilisateur?.id);
  const myRoundResult = myBet?.result === 'WIN' ? 'GAGNE' : myBet?.result === 'LOSS' ? 'PERDUE' : null;
  const multiplierFor = (answer: 'FACT' | 'FAKE') => etat?.multipliers?.[answer] ?? null;

  useEffect(() => {
    if (etat?.room?.status !== 'FINISHED' || !roomId || !finishedAt) return;
    if (finishedRemaining <= 0) {
      navigate('/play', { replace: true });
      return;
    }
    const timer = window.setTimeout(() => {
      navigate('/play', { replace: true });
    }, finishedRemaining + 50);
    return () => window.clearTimeout(timer);
  }, [etat?.room?.status, roomId, finishedAt, finishedRemaining, navigate]);

  useEffect(() => {
    if (etat?.room?.status !== 'FINISHED' || !roomId || finalResults) return;
    // Un seul récapitulatif par salon : sans ce garde-fou l'effet repartait à
    // chaque rendu (identité de `members` instable, `now` change toutes les
    // 250 ms), son nettoyage remettait `actif` à false avant la résolution, et
    // /ai/claims/:id/verdict était rejoué en boucle.
    if (finalResultsRoomRef.current === roomId) return;
    finalResultsRoomRef.current = roomId;
    let actif = true;
    Promise.all([
      api<any>(`/rooms/${roomId}/history`),
      api<any[]>('/stats/leaderboard').catch(() => []),
    ]).then(async ([history, classement]) => {
      const rounds = [...(history.rounds ?? [])].sort((a: any, b: any) =>
        (a.createdAt ?? '').localeCompare(b.createdAt ?? ''),
      );
      const clipResults: ClipResult[] = [];
      for (const gameRound of rounds) {
        const rc = gameRound.roundClaims?.[0];
        const claim = rc?.claim;
        if (!claim) continue;
        let verdictText = '';
        let citations: string[] = [];
        try {
          const verdict = await api<any>(`/ai/claims/${claim.id}/verdict`);
          verdictText = verdict?.data?.verdictText ?? '';
          citations = Array.isArray(verdict?.data?.citations) ? verdict.data.citations : [];
        } catch {}
        const bet = (gameRound.bets ?? []).find((b: any) => b.userId === utilisateur?.id);
        clipResults.push({
          id: claim.id,
          verdict: claim.truthLabel === 'TRUE' ? 'VRAI' : 'FAUX',
          title: claim.text,
          explanation: verdictText || 'Vérification éditoriale disponible.',
          source: citations.join(', ') || claim.sourceUrl || 'Source éditoriale',
          explanationSource: 'editorial',
          userVote: bet?.answer === 'FACT' ? 'VRAI' : bet?.answer === 'FAKE' ? 'FAUX' : undefined,
        });
      }
      const debutPartie = rounds[0]?.createdAt ? new Date(rounds[0].createdAt).getTime() : Number.NEGATIVE_INFINITY;
      const entries = membersRef.current.map((member: any) => {
        const bets = rounds.flatMap((gameRound: any) => gameRound.bets ?? []).filter((b: any) => b.userId === member.id);
        const statsJoueur = classement.find((entry: any) => entry?.user?.id === member.id);
        const achievementsUnlocked = (statsJoueur?.user?.achievements ?? [])
          .filter((achievement: any) => new Date(achievement.unlockedAt).getTime() >= debutPartie)
          .map((achievement: any) => achievement.achievementKey);
        return {
          userId: member.id,
          username: member.displayName ?? member.email ?? 'joueur',
          correctCount: bets.filter((b: any) => b.result === 'WIN').length,
          creditsChange: bets.reduce((sum: number, b: any) => sum + ((b.payout ?? 0) - (b.stake ?? 0)), 0),
          achievementsUnlocked,
        };
      }).sort((a, b) => b.creditsChange - a.creditsChange || b.correctCount - a.correctCount || a.userId.localeCompare(b.userId));
      const meEntry = entries.find((entry) => entry.userId === utilisateur?.id);
      if (actif) setFinalResults({
        correctCount: meEntry?.correctCount ?? 0,
        clipResults,
        leaderboard: entries,
      });
    }).catch(() => { finalResultsRoomRef.current = null; });
    return () => { actif = false; };
  }, [etat?.room?.status, roomId, utilisateur?.id, finalResults]);

  useEffect(() => {
    setRoundTruth(null);
    if (!round?.id) { setStake(null); setCustomStake(''); setStakeValidationError(null); setAmountConfirmed(false); setAnswered(null); return; }
    const existing = (round.bets ?? []).find((b: any) => b.userId === utilisateur?.id);
    if (existing) {
      setStake(existing.stake ?? null);
      setCustomStake(existing.stake != null ? String(existing.stake) : '');
      setStakeValidationError(null);
      setAmountConfirmed(Boolean(existing.amountConfirmedAt));
      setAnswered(existing.answer ?? null);
    } else {
      setStake(null); setCustomStake(''); setStakeValidationError(null); setAmountConfirmed(false); setAnswered(null);
    }
  }, [round?.id, round?.bets, utilisateur?.id]);

  useEffect(() => {
    if (!connecte) return;
    const cleanups = [
      ecouter('balance:update', (d: any) => { if (d?.userId === utilisateur?.id) { setBalance(d.virtualBalance); setRecoveryAvailable(Boolean(d.recoveryAvailable)); } }),
      ecouter('round:verdict', (d: any) => {
        if (d?.truth === 'FACT' || d?.truth === 'FAKE') setRoundTruth(d.truth);
      }),
      ecouter('room:expired', () => { setActionError('Le lobby a expiré avant que tout le monde soit prêt.'); window.setTimeout(() => navigate('/play'), 1200); }),
      ecouter('room:destroyed', () => { navigate('/play'); }),
      ecouter('game:cancelled', () => { navigate('/play'); }),
      ecouter('room:kicked', (d: any) => {
        const kicked = d?.userId === utilisateur?.id
          || (Array.isArray(d?.userIds) && d.userIds.includes(utilisateur?.id));
        if (kicked) {
          setActionError('La partie est terminée. Retour à la page Play…');
          window.setTimeout(() => navigate('/play', { replace: true }), 400);
        }
      }),
      ecouter('chat:message', (d: any) => { const m = d?.message; if (!m) return; setMessages((prev) => [...prev, { id: m.id, type: 'user', author: m.user?.displayName || m.user?.email?.split('@')[0] || 'joueur', content: m.content, timestamp: new Date(m.createdAt).getTime() }]); }),
      ecouter('room:joined', () => { api<any[]>(`/chat/room/${roomId}`).then((list) => setMessages(list.map((m) => ({ id: m.id, type: 'user' as const, author: m.user?.displayName || m.user?.email?.split('@')[0] || 'joueur', content: m.content, timestamp: new Date(m.createdAt).getTime() })))).catch(() => {}); }),
      ecouter('chat:moderated', (d: any) => {
        setMessages((prev) => [...prev, {
          id: `mod-${Date.now()}-${Math.random().toString(36).slice(2, 8)}`,
          type: 'system' as const,
          author: 'modération',
          content: d?.reason || 'Message bloqué par la modération.',
          timestamp: Date.now(),
          moderated: true,
        }]);
      }),
      ecouter('ai:sources', (d: any) => {
        const sources = (Array.isArray(d) ? d : [])
          .map((s: any) => ({ title: typeof s?.title === 'string' ? s.title : undefined, url: s?.url }))
          .filter((s: any): s is { title?: string; url: string } => typeof s.url === 'string' && s.url.length > 0);
        if (sources.length === 0) return;
        setMessages((prev) => majMessageBot(prev, botStreamRef, (msg) => ({ ...msg, sources })));
      }),
      ecouter('ai:token', (d: any) => {
        const text = typeof d?.text === 'string' ? d.text : '';
        if (!text) return;
        setMessages((prev) => majMessageBot(prev, botStreamRef, (msg) => ({ ...msg, content: msg.content + text })));
      }),
      ecouter('ai:error', (d: any) => {
        const texte = typeof d?.message === 'string' && d.message.trim()
          ? d.message
          : 'La réponse de l’IA a été interrompue.';
        setMessages((prev) => majMessageBot(prev, botStreamRef, (msg) => ({ ...msg, content: msg.content || texte })));
        botStreamRef.current = null;
      }),
      ecouter('ai:done', () => { botStreamRef.current = null; }),
    ];
    return () => cleanups.forEach((off) => off());
  }, [connecte, ecouter, navigate, roomId, utilisateur?.id]);

  const betMinimum = balance >= 10 ? 10 : balance;
  const availableOptions = useMemo(() => BET_OPTIONS.filter((amount) => amount <= balance), [balance]);

  async function confirmStake(value = stake) {
    if (!roomId || !round?.id || !value) return;
    setStakeValidationError(null);
    setActionError(null);
    if (!Number.isSafeInteger(value) || value < betMinimum || value > balance) {
      setStakeValidationError(`Choisis un montant entier compris entre ${betMinimum} et ${balance} crédits.`);
      return;
    }
    try {
      await demander('bet:confirm', { roomId, roundId: round.id, stake: value });
      setStake(value);
      setCustomStake(String(value));
      setAmountConfirmed(true);
    } catch (e) {
      setStakeValidationError(e instanceof Error ? e.message : 'Montant de mise invalide');
    }
  }

  function choisirMise(value: number) {
    if (value > balance || answered) return;
    setStake(value);
    setCustomStake(String(value));
    void confirmStake(value);
  }

  function validerMisePersonnalisee() {
    const value = Number(customStake.trim());
    setStakeValidationError(null);
    if (!customStake.trim() || !Number.isSafeInteger(value)) {
      setStakeValidationError(`Entre un nombre entier compris entre ${betMinimum} et ${balance} crédits.`);
      return;
    }
    void confirmStake(value);
  }

  async function signalerVideoIndisponible() {
    if (!roomId || !round?.id) return;
    try {
      await demander('round:video-unavailable', { roomId, roundId: round.id });
    } catch (e) {
      setActionError(e instanceof Error ? e.message : 'Impossible de signaler la vidéo');
    }
  }

  async function answer(value: 'FACT' | 'FAKE') {
    if (!roomId || !round?.id || !stake || !amountConfirmed || answered) return;
    setActionError(null);
    try {
      await demander('answer:submit', { roomId, roundId: round.id, answer: value });
      setAnswered(value);
    } catch (e) { setActionError(e instanceof Error ? e.message : 'Impossible de valider la réponse'); }
  }

  async function ready() {
    try { await demander('room:ready', { roomId }); } catch (e) { setActionError(e instanceof Error ? e.message : 'Impossible de se déclarer prêt-e'); }
  }

  async function recover() {
    if (recoveryInProgress || !recoveryVisible || balance !== 0) return;
    setRecoveryInProgress(true);
    setActionError(null);
    try {
      const result = await demander<{ virtualBalance: number }>('bankrupt:claim', {});
      setBalance(result.virtualBalance);
      setRecoveryAvailable(false);
    } catch (e) {
      setActionError(e instanceof Error ? e.message : 'Récupération impossible');
    } finally {
      setRecoveryInProgress(false);
    }
  }

  async function leave() {
    if (!roomId) return;
    try {
      await demander('room:leave', { roomId }, { reconnect: true });
      deconnecterVolontairement();
    } catch (e) {
      const message = e instanceof Error ? e.message : 'Impossible de quitter le salon';
      setActionError(message);
      throw e;
    }
  }

  useLayoutEffect(() => {
    registerRoomLeave(leave);
    return () => registerRoomLeave(null);
  }, [registerRoomLeave, roomId, demander, deconnecterVolontairement]);

  if (!roomId) return <div className="p-8 text-white">Salon introuvable.</div>;

  const roomLoaded = Boolean(etat?.room);
  if (!roomLoaded) {
    return (
      <section className="min-h-full flex flex-1 items-center justify-center bg-noir-krystal px-4 text-white">
        <p className="text-center text-lg font-medium text-white/70">Chargement de la partie...</p>
      </section>
    );
  }

  const firstRoundEnded = Boolean(roomStats?.roundDetails?.some((detail) => ['REVEALED', 'FINISHED'].includes(detail.status)));
  const leader = roomStats?.players?.[0];
  const activePlayers = members.filter((member) => ['PLAYING', 'READY'].includes(member.state)).length;
  const currentRoundNumber = Math.min(5, gameRoundIndex + 1);
  const isBetweenRounds = round?.status === 'REVEALED';

  return (
    <section className="min-h-full flex flex-1 items-center justify-center bg-noir-krystal p-4 md:p-6 text-white">
      <div className="w-full max-w-[1400px] space-y-5">
        {lobby && (
          <div className="mx-auto w-full max-w-[1100px] space-y-5">
            <section className="mx-auto w-full max-w-[700px] rounded-2xl bg-gris-krystal p-6 border border-white/10 text-center">
              <h1 className="text-3xl font-bold tracking-[0.16em]">FACTARENA</h1>
              <p className="mt-3 text-xl font-semibold">En attente des joueurs...</p>
              <p className="mx-auto mt-2 max-w-3xl text-sm text-white/65">
                La partie peut commencer dès qu’il y a au moins 2 joueurs et que chacun d'entre eux soit prêt.
              </p>
              <p className="mx-auto mt-2 max-w-2xl text-xs text-white/35">
                Si le temps imparti arrive à son terme, tous les joueurs sont expulsés et la partie est annulée.
              </p>
            </section>

            <section className="mx-auto w-full max-w-[200px] rounded-2xl bg-gris-krystal p-5 border border-white/10 text-center">
              <p className="text-xs uppercase tracking-[0.18em] text-white/40">Temps restant</p>
              <p className="mt-1 text-3xl font-bold tabular-nums">{formatCountdown(lobbyRemaining)}</p>
            </section>

            <div className="grid gap-5 lg:grid-cols-[minmax(0,2fr)_minmax(0,1fr)]">
              <section className="rounded-2xl bg-gris-krystal p-5 border border-white/10">
                <h2 className="text-xl font-semibold text-center">Participants de la partie</h2>
                <div className="mt-5 grid gap-3 sm:grid-cols-2">
                  {members.map((member) => {
                    const kickable = member.id !== utilisateur?.id && !member.ready && Date.now() - new Date(member.joinedAt).getTime() >= 45_000;
                    return (
                      <div key={member.id} className="flex items-center gap-3 rounded-xl bg-noir-krystal p-3 border border-white/5">
                        {member.avatarUrl ? (
                          <img src={member.avatarUrl} className="h-10 w-10 rounded-full object-cover shrink-0" alt="" />
                        ) : (
                          <div className="h-10 w-10 rounded-full bg-grisclair-krystal flex items-center justify-center font-semibold shrink-0">
                            {(member.displayName ?? member.email ?? '?').charAt(0).toUpperCase()}
                          </div>
                        )}
                        <div className="min-w-0 flex-1">
                          <div className="flex items-center gap-2 min-w-0">
                            <p className="truncate font-medium">{member.displayName ?? member.email}</p>
                            <span className="ml-auto shrink-0 text-xs font-semibold text-amber-300">{member.virtualBalance ?? 0} cr</span>
                          </div>
                          <p className={`text-xs ${member.ready ? 'text-emerald-400' : 'text-white/40'}`}>{member.ready ? 'PRÊT-E' : 'En attente'}</p>
                        </div>
                        {kickable && (
                          <button onClick={() => demander('room:kick', { roomId, targetUserId: member.id }).catch((e) => setActionError(e.message))} className="text-xs text-red-300 hover:text-red-200">
                            kick
                          </button>
                        )}
                      </div>
                    );
                  })}
                </div>
                <div className="mt-6 flex items-center justify-center">
                  <div className="flex flex-wrap items-center justify-center gap-3">
                    <span className="text-sm font-semibold text-white/65">{me?.ready ? 'Prêt.e à jouer' : 'Prêt.e à jouer ?'}</span>
                    <button disabled={Boolean(me?.ready)} onClick={ready} className="button-blue-d px-6 py-3">
                      {me?.ready ? 'PRÊT-E ✓' : 'JE SUIS PRÊT-E'}
                    </button>
                  </div>
                </div>
              </section>
              <div className="min-h-[420px]"><Chat messages={messages} roomLabel="Chat du Lobby" onSendMessage={(content) => emettre('chat:send', { roomId, content })} /></div>
            </div>
          </div>
        )}

        {game && round && claim && (
          <div className="mx-auto w-full max-w-[1400px] space-y-5">
            <section className="mx-auto w-full max-w-[700px] rounded-2xl bg-gris-krystal p-5 border border-white/10 text-center">
              <h1 className="text-4xl font-bold tracking-[0.16em]">FACTARENA</h1>
            </section>

            <section className="rounded-2xl bg-gris-krystal border border-white/10 overflow-hidden">
              <button type="button" onClick={() => setRoomStatsOpen(v => !v)}
                className="premium-vote premium-bourse relative w-full flex flex-col items-center justify-center gap-1 p-3 text-center hover:bg-white/[0.02]" aria-expanded={roomStatsOpen}>
                <span className="font-semibold tracking-[0.17em] text-amber-100">LA BOURSE</span>
                <span aria-hidden="true" className={`h-2.5 w-2.5 border-r border-b border-white/70 transition-transform ${roomStatsOpen ? 'rotate-[225deg] translate-y-0.5' : 'rotate-45 -translate-y-0.5'}`} />
              </button>
              {roomStatsOpen && <div className="border-t border-white/10 p-4 sm:p-5">
                {!roomStats ?
                 roomStatsLoading ? <p className="py-8 text-center text-sm text-white/40">Chargement…</p> :
                 roomStatsError ? <p className="py-8 text-center text-sm text-red-200">Statistiques indisponibles. La partie continue normalement.</p> :
                 <p className="py-8 text-center text-sm text-white/35">Les statistiques apparaîtront après les premières réponses.</p> :
                 <div className="grid gap-4 lg:grid-cols-[minmax(0,3fr)_minmax(280px,2fr)]">
                   <div className="rounded-2xl border border-white/10 bg-noir-krystal/75 p-4 sm:p-5 shadow-[0_18px_50px_rgba(0,0,0,.18)]">
                     <div className="flex items-end justify-between gap-3 mb-1"><div><p className="text-[10px] uppercase tracking-[.22em] text-white/35">Patrimoine</p><h3 className="text-base font-semibold">Évolution des crédits par joueur</h3></div><span className="text-[10px] text-white/35">crédits · mises</span></div>
                     <RoomCreditsChart players={roomStats.players} roundDetails={roomStats.roundDetails} evolution={roomStats.evolution} />
                   </div>
                   <div className="grid gap-4">
                     <div className="rounded-2xl border border-white/10 bg-noir-krystal/75 p-4 shadow-[0_18px_50px_rgba(0,0,0,.18)]"><div className="mb-1"><p className="text-[10px] uppercase tracking-[.22em] text-white/35">Performance</p><h3 className="text-sm font-semibold">Précision cumulée</h3></div><RoomAccuracyChart players={roomStats.players} roundDetails={roomStats.roundDetails} /></div>
                     <div className="rounded-2xl border border-white/10 bg-noir-krystal/75 p-4 shadow-[0_18px_50px_rgba(0,0,0,.18)]"><div className="mb-1"><p className="text-[10px] uppercase tracking-[.22em] text-white/35">Résultats</p><h3 className="text-sm font-semibold">Gains / pertes cumulés</h3></div><RoomCumulativeGainChart players={roomStats.players} roundDetails={roomStats.roundDetails} /></div>
                   </div>
                 </div>}
              </div>}
            </section>

            {erreur || actionError ? <div className="rounded-xl border border-red-400/20 bg-red-400/10 px-4 py-3 text-sm text-red-200">{erreur ?? actionError}</div> : null}

            <section className="grid grid-cols-2 md:grid-cols-5 gap-3">
              <div className="rounded-xl bg-gris-krystal border border-white/10 p-3 text-center min-w-0">
                <p className="text-[11px] uppercase tracking-wider text-white/40">Joueur en tête</p>
                <p className="mt-1 truncate font-semibold">{firstRoundEnded ? (leader?.user.displayName || leader?.user.email || '-') : '-'}</p>
              </div>
              <div className="rounded-xl bg-gris-krystal border border-white/10 p-3 text-center">
                <p className="text-[11px] uppercase tracking-wider text-white/40">Manche</p>
                <p className="mt-1 text-lg font-bold tabular-nums">{currentRoundNumber} / 5</p>
              </div>
              <div className="rounded-xl bg-gris-krystal border border-white/10 p-3 text-center">
                <p className="text-[11px] uppercase tracking-wider text-white/40">Joueurs actifs</p>
                <p className="mt-1 text-lg font-bold tabular-nums">{activePlayers}</p>
              </div>
              <div className="rounded-xl bg-gris-krystal border border-white/10 p-3 text-center">
                <p className="text-[11px] uppercase tracking-wider text-white/40">Temps restant</p>
                <p className="mt-1 text-lg font-bold tabular-nums">{formatCountdown(roundRemaining)}</p>
              </div>
              <div className="rounded-xl bg-gris-krystal border border-white/10 p-3 text-center">
                <p className="text-[11px] uppercase tracking-wider text-white/40">Tes crédits</p>
                <p className="mt-1 text-lg font-bold tabular-nums">{balance} cr</p>
              </div>
            </section>

            <div className="grid gap-5 xl:grid-cols-[minmax(0,1fr)_380px] items-stretch">
              <div className="min-w-0 space-y-5">
                <div className="overflow-hidden rounded-2xl bg-black border border-white/10">
                  <VideoPlayer src={claim.mediaRef} onUnavailable={signalerVideoIndisponible} />
                </div>

                <section className="rounded-2xl bg-gris-krystal p-5 border border-white/10">
                  <h2 className="text-lg font-medium leading-relaxed text-center">{claim.text}</h2>
                </section>

                <section className="rounded-2xl bg-gris-krystal border border-white/10 p-4 sm:p-5 min-h-[300px] flex items-center">
                  {isBetweenRounds ? (
                    <div className={`w-full min-h-[250px] flex flex-col items-center justify-center rounded-xl border ${myRoundResult === 'GAGNE' ? 'border-emerald-400/25 bg-emerald-400/10' : 'border-red-400/25 bg-red-400/10'}`}>
                      {myRoundResult === 'GAGNE' ? (
                        <p className="text-lg sm:text-xl font-semibold text-emerald-300 text-center">Bonne réponse - Tu as gagné {myBet?.payout ?? 0} crédits cette manche.
						</p>
                      ) : (
                        <p className="text-lg sm:text-xl font-semibold text-red-300 text-center">Mauvaise réponse - Tu as perdu ta mise{myBet?.stake != null ? ` de ${myBet.stake} crédits` : ''} cette manche.
						</p>
                      )}
                    </div>
                  ) : balance === 0 ? (
                    <div className="w-full min-h-[250px] flex flex-col items-center justify-center rounded-xl border border-amber-300/20 bg-amber-300/10 text-center px-5">
                      <p className="font-semibold">Solde nul</p>
                      <p className="mt-1 text-sm text-white/60">
                        {recoveryAvailable ? 'Tu peux récupérer 200 crédits.' : 'La récupération sera disponible après le règlement de la manche.'}
                      </p>
                      {recoveryVisible && (
                        <button disabled={recoveryInProgress} onClick={recover} className="button-blue-d mt-4 px-6 py-3 disabled:opacity-50">
                          {recoveryInProgress ? '…' : 'Récupérer 200 crédits'}
                        </button>
                      )}
                    </div>
                  ) : (
                    <div className="w-full min-h-[180px] md:min-h-[250px] grid grid-cols-1 md:grid-cols-[minmax(105px,1fr)_minmax(230px,1.65fr)_minmax(105px,1fr)] gap-3 items-stretch">
                      <button disabled={!amountConfirmed || Boolean(answered) || roundRemaining <= 0} onClick={() => answer('FACT')} className="premium-vote premium-vote-fact min-h-[100px] md:min-h-[250px] rounded-2xl text-2xl sm:text-3xl font-extrabold tracking-[0.12em] flex-col disabled:opacity-35">
                        <span>FACT</span>
                        <span className="mt-2 text-xs sm:text-sm font-medium tracking-normal opacity-80 block">× {multiplierFor('FACT') === null ? '---' : multiplierFor('FACT')!.toFixed(2)}</span>
                      </button>

                      <div className="rounded-2xl border border-white/10 bg-noir-krystal p-3 flex flex-col justify-center">
                        <p className="text-[10px] uppercase tracking-[0.18em] text-white/40 text-center">Montant de la mise</p>
                        <div className="mt-3 grid grid-cols-2 gap-2">
                          {availableOptions.map((amount) => (
                            <button key={amount} disabled={Boolean(answered)} onClick={() => choisirMise(amount)}
                              className={`rounded-lg border px-2 py-2.5 text-xs sm:text-sm font-semibold transition ${stake === amount && amountConfirmed ? 'border-bleu-l bg-bleu-d text-white' : 'border-white/10 bg-gris-krystal text-white/70 hover:text-white hover:border-white/25'}`}>
                              {amount} crédits
                            </button>
                          ))}
                          {balance > 0 && (
                            <button disabled={Boolean(answered)} onClick={() => choisirMise(balance)}
                              className={`rounded-lg border px-2 py-2.5 text-xs sm:text-sm font-semibold transition ${stake === balance && amountConfirmed ? 'border-bleu-l bg-bleu-d text-white' : 'border-white/10 bg-gris-krystal text-white/70 hover:text-white hover:border-white/25'}`}>
                              Tout
                            </button>
                          )}
                        </div>
                        <div className="mt-2">
                          <label htmlFor="custom-stake" className="sr-only">Montant personnalisé</label>
                          <input
                            id="custom-stake"
                            type="number"
                            inputMode="numeric"
                            min={betMinimum}
                            max={balance}
                            step="1"
                            value={customStake}
                            disabled={Boolean(answered)}
                            onChange={(event) => { setCustomStake(event.target.value); setStakeValidationError(null); }}
                            onBlur={validerMisePersonnalisee}
                            onKeyDown={(event) => { if (event.key === 'Enter') { event.preventDefault(); validerMisePersonnalisee(); } }}
                            placeholder={balance < 10 ? `Tout miser : ${balance} crédits` : `${betMinimum} à ${balance} crédits`}
                            className={`w-full rounded-lg border bg-gris-krystal px-3 py-2.5 text-center text-sm outline-none placeholder:text-white/25 ${stakeValidationError ? 'border-red-400/70' : 'border-white/10 focus:border-bleu-l'}`}
                          />
                          {stakeValidationError && <p className="mt-1.5 text-center text-[11px] text-red-300">{stakeValidationError}</p>}
                          {!stakeValidationError && balance < 10 && <p className="mt-1.5 text-center text-[11px] text-white/35">Solde inférieur à 10 crédits : mise totale uniquement.</p>}
                        </div>
                        {answered && <p className="mt-3 text-center text-xs text-emerald-300">Réponse verrouillée : {answered}</p>}
                      </div>

                      <button disabled={!amountConfirmed || Boolean(answered) || roundRemaining <= 0} onClick={() => answer('FAKE')} className="premium-vote premium-vote-fake min-h-[100px] md:min-h-[250px] rounded-2xl text-2xl sm:text-3xl font-extrabold tracking-[0.12em] flex-col disabled:opacity-35">
                        <span>FAKE</span>
                        <span className="mt-2 text-xs sm:text-sm font-medium tracking-normal opacity-80 block">× {multiplierFor('FAKE') === null ? '---' : multiplierFor('FAKE')!.toFixed(2)}</span>
                      </button>
                    </div>
                  )}
                </section>
              </div>

              <aside className="min-h-[700px] h-full"><Chat messages={messages} roomLabel="Chat" onSendMessage={(content) => emettre('chat:send', { roomId, content })} /></aside>
            </div>

            <RoomFriends />
          </div>
        )}
        {etat?.room?.status === 'FINISHED' && (
          <div className="mx-auto w-full max-w-[1300px]">
            {finalResults ? (
              <>
                <section className="rounded-2xl bg-gris-krystal p-5 border border-white/10 text-center mb-5">
                  <h1 className="text-3xl font-bold tracking-[0.16em]">FACTARENA</h1>
                </section>
                <div className="grid gap-5 xl:grid-cols-[minmax(0,1fr)_380px] items-stretch">
                  <ResultsReveal
                    correctCount={finalResults.correctCount}
                    totalClips={finalResults.clipResults.length}
                    clipResults={finalResults.clipResults}
                    leaderboard={finalResults.leaderboard}
                    isWinner={finalResults.leaderboard.some((entry) => entry.userId === utilisateur?.id && entry.creditsChange === Math.max(...finalResults.leaderboard.map((item) => item.creditsChange)))}
                    hideLeaderboard
                  />
                  <aside className="h-full min-h-[420px]"><Chat messages={messages} roomLabel="Chat" onSendMessage={(content) => emettre('chat:send', { roomId, content })} /></aside>
                </div>
                <div className="mt-5 rounded-2xl bg-gris-krystal border border-white/10 overflow-hidden">
                  <div className="flex flex-wrap items-center justify-between gap-3 border-b border-white/10 px-5 py-4">
                    <div><p className="text-[10px] uppercase tracking-[.22em] text-white/35">Récapitulatif financier</p><h2 className="font-semibold tracking-[.14em] text-amber-100">LA BOURSE</h2></div>
                    <div className="flex gap-2">
                      {roomStats && <><button type="button" onClick={() => exportRoomCsv(roomStats)} className="button-blue-d px-3 py-2 text-xs">CSV</button><button type="button" onClick={() => exportRoomPdf(roomStats)} className="button-blue-d px-3 py-2 text-xs">PDF</button></>}
                    </div>
                  </div>
                  <div className="p-4 sm:p-5">
                    {roomStats ? <div className="grid gap-4 lg:grid-cols-[minmax(0,3fr)_minmax(280px,2fr)]"><div className="rounded-2xl border border-white/10 bg-noir-krystal/75 p-4"><h3 className="font-semibold">Évolution des crédits par joueur</h3><RoomCreditsChart players={roomStats.players} roundDetails={roomStats.roundDetails} evolution={roomStats.evolution} /></div><div className="grid gap-4"><div className="rounded-2xl border border-white/10 bg-noir-krystal/75 p-4"><h3 className="font-semibold">Précision cumulée</h3><RoomAccuracyChart players={roomStats.players} roundDetails={roomStats.roundDetails} /></div><div className="rounded-2xl border border-white/10 bg-noir-krystal/75 p-4"><h3 className="font-semibold">Gains / pertes cumulés</h3><RoomCumulativeGainChart players={roomStats.players} roundDetails={roomStats.roundDetails} /></div></div></div> : <p className="py-8 text-center text-sm text-white/40">Les statistiques financières sont indisponibles.</p>}
                  </div>
                </div>
                <div className="mt-5 grid gap-5 md:grid-cols-2">
                  <LeaderboardPanel leaderboard={finalResults.leaderboard} />
                  <RoomFriends />
                </div>
                <div className="mt-5 rounded-2xl bg-gris-krystal p-4 border border-white/10 text-center">
                  <p className="text-xs uppercase tracking-widest text-white/40">FIN DU SALON</p>
                  <p className="mt-1 text-lg font-semibold tabular-nums">{formatCountdown(finishedRemaining)}</p>
                </div>
              </>
            ) : (
              <div className="rounded-2xl bg-gris-krystal p-6 border border-white/10 text-center text-white/60">Chargement du récapitulatif…</div>
            )}
          </div>
        )}
      </div>
    </section>
  );
}

function LeaderboardPanel({ leaderboard }: { leaderboard: LeaderboardEntry[] }) {
  const meilleurGain = leaderboard.length > 0 ? Math.max(...leaderboard.map((entry) => entry.creditsChange)) : Number.NEGATIVE_INFINITY;
  return (
    <section className="bg-gris-krystal rounded-2xl p-5 border border-white/10">
      <h3 className="text-white font-semibold text-center mb-4">Classement des joueurs et gains</h3>
      <ul className="space-y-2">
        {['1er', '2e', '3e', '4e', '5e'].map((position, index) => {
          const entry = leaderboard[index];
          const estGagnant = entry !== undefined && entry.creditsChange === meilleurGain;
          return (
            <li key={position} className={`flex items-center gap-3 rounded-lg border px-3 py-2 text-sm ${estGagnant ? 'bg-amber-400/10 border-amber-300/20' : 'bg-white/[0.03] border-white/5'}`}>
              <span className={`w-10 shrink-0 font-semibold ${estGagnant ? 'text-amber-300' : 'text-white/60'}`}>{position}</span>
              <span className="flex-1 min-w-0 truncate text-white/90">{entry?.username ?? '—'}</span>
              {entry && <span className={`shrink-0 font-semibold ${entry.creditsChange >= 0 ? 'text-emerald-400' : 'text-red-400'}`}>{entry.creditsChange >= 0 ? '+' : ''}{entry.creditsChange} crédits</span>}
            </li>
          );
        })}
      </ul>
    </section>
  );
}

function RoomFriends() {
  const [recherche, setRecherche] = useState('');
  const [resultats, setResultats] = useState<Ami[]>([]);
  const [recues, setRecues] = useState<(Ami & { id: string })[]>([]);
  const [envoyees, setEnvoyees] = useState<(Ami & { id: string })[]>([]);
  const [message, setMessage] = useState<string | null>(null);

  const rafraichirDemandes = () => {
    demandesAmis().then((demandes) => { setRecues(demandes.received); setEnvoyees(demandes.sent); }).catch(() => {});
  };

  useEffect(() => {
    rafraichirDemandes();
    const update = () => rafraichirDemandes();
    window.addEventListener('factarena:friend-update', update);
    return () => window.removeEventListener('factarena:friend-update', update);
  }, []);

  async function chercher(event: React.FormEvent) {
    event.preventDefault();
    const texte = recherche.trim();
    if (texte.length < 2) { setMessage('Entre au moins 2 caractères.'); return; }
    setMessage(null);
    try { setResultats(await chercherJoueurs(texte)); } catch (e) { setMessage(e instanceof Error ? e.message : 'Recherche impossible'); }
  }

  async function ajouter(joueur: Ami) {
    setMessage(null);
    try { await ajouterAmi(joueur.id); setResultats((items) => items.filter((item) => item.id !== joueur.id)); rafraichirDemandes(); }
    catch (e) { setMessage(e instanceof Error ? e.message : 'Ajout impossible'); }
  }

  async function accepter(request: Ami & { id: string }) {
    try { await accepterDemande(request.id); rafraichirDemandes(); } catch (e) { setMessage(e instanceof Error ? e.message : 'Acceptation impossible'); }
  }

  async function refuser(request: Ami & { id: string }) {
    try { await refuserDemande(request.id); rafraichirDemandes(); } catch (e) { setMessage(e instanceof Error ? e.message : 'Refus impossible'); }
  }

  return (
    <section className="mx-auto w-full max-w-2xl rounded-2xl bg-gris-krystal p-4 sm:p-5 border border-white/10">
      <h2 className="font-semibold text-center">Demande d'amis</h2>
      <form onSubmit={chercher} className="mt-4 flex gap-2 max-w-2xl mx-auto">
        <input type="text" value={recherche} onChange={(e) => setRecherche(e.target.value)} placeholder="e-mail ou pseudo" minLength={2} maxLength={60} className="flex-1 min-w-0 bg-noir-krystal rounded-lg px-3 py-2 text-sm outline-none placeholder:text-white/25" />
        <button type="submit" className="button-blue-d px-4 py-2 text-sm">chercher</button>
      </form>
      {message && <p className="mt-2 text-center text-red-l text-sm">{message}</p>}
      {resultats.length > 0 && (
        <ul className="mt-4 grid gap-2 sm:grid-cols-2">
          {resultats.map((joueur) => (
            <li key={joueur.id} className="bg-noir-krystal rounded-xl p-3 flex items-center gap-3">
              <span className="min-w-0 flex-1 truncate text-sm font-semibold">{joueur.displayName || joueur.email}</span>
              <button type="button" onClick={() => ajouter(joueur)} className="text-xs text-bleu-l hover:underline">ajouter</button>
            </li>
          ))}
        </ul>
      )}
      <div className="mt-5 grid gap-4 md:grid-cols-2">
        <div>
          <h3 className="text-sm font-semibold mb-2">Invitations reçues</h3>
          {recues.length === 0 ? <p className="text-xs text-white/35">Aucune invitation en attente.</p> : <ul className="space-y-2">{recues.map((demande) => <li key={demande.id} className="bg-noir-krystal rounded-xl p-3 flex items-center gap-3"><span className="min-w-0 flex-1 truncate text-sm">{demande.displayName || demande.email}</span><button type="button" onClick={() => accepter(demande)} className="text-xs text-bleu-l hover:underline">accepter</button><button type="button" onClick={() => refuser(demande)} className="text-xs text-white/40 hover:text-red-l">refuser</button></li>)}</ul>}
        </div>
        <div>
          <h3 className="text-sm font-semibold mb-2">Invitations envoyées</h3>
          {envoyees.length === 0 ? <p className="text-xs text-white/35">Aucune invitation en attente.</p> : <ul className="space-y-2">{envoyees.map((demande) => <li key={demande.id} className="bg-noir-krystal rounded-xl p-3 flex items-center gap-3"><span className="min-w-0 flex-1 truncate text-sm">{demande.displayName || demande.email}</span><span className="text-xs text-white/40">en attente...</span></li>)}</ul>}
        </div>
      </div>
    </section>
  );
}
