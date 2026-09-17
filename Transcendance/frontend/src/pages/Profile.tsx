import { useCallback, useEffect, useRef, useState } from 'react';
import { useParams } from 'react-router';
import { api, chercherJoueurs, envoyerAvatar, modifierProfil, reclamerRattrapage, statistiquesDashboard, type ProfilJoueur, type StatsDashboard } from '../api/client';
import PeriodSelector, { type PeriodValue } from '../components/stats/PeriodSelector';
import { AccuracyRadar, BalanceEvolution, GainLossPie, EmptyStats } from '../components/stats/Charts';
import { exportPersonalCsv, exportPersonalPdf } from '../components/stats/export';
import { useAuth } from '../auth/AuthContext';
import { ACHIEVEMENTS } from '../mocks/gameData';


interface Stats {
  user: ProfilJoueur;
  virtualBalance: number;
  recoveryAvailable: boolean;
  roundsPlayed: number;
  victories: number;
  defeats: number;
  totalStake: number;
  totalPayout: number;
  netGain: number;
  winRate: number;
}


interface MancheJouee {
  id: string;
  stake: number;
  payout: number | null;
  result: 'WIN' | 'LOSS' | 'CANCELLED' | null;
  createdAt: string;
  round: {
    id: string;
    revealedAt: string | null;
    createdAt: string;
    room: { id: string; name: string } | null;
    bets: { userId: string; result: 'WIN' | 'LOSS' | 'CANCELLED' | null; user: ProfilJoueur }[];
  };
}


export default function Profile() {
  const { userId } = useParams<{ userId: string }>();
  const { utilisateur, rafraichir } = useAuth();
  const profilId = userId ?? utilisateur?.id;
  const estMonProfil = !userId || userId === utilisateur?.id;
  const [stats, setStats] = useState<Stats | null>(null);
  const [historique, setHistorique] = useState<MancheJouee[]>([]);

  const [edition, setEdition] = useState(false);
  const [nouveauPseudo, setNouveauPseudo] = useState('');
  const [messageProfil, setMessageProfil] = useState<string | null>(null);
  const [rattrapageEnCours, setRattrapageEnCours] = useState(false);
  const [periode, setPeriode] = useState<PeriodValue>('all');
  const [dateDebut, setDateDebut] = useState('');
  const [dateFin, setDateFin] = useState('');
  const [dashboard, setDashboard] = useState<StatsDashboard | null>(null);
  const [dashboardLoading, setDashboardLoading] = useState(false);
  const [dashboardError, setDashboardError] = useState(false);
  const [periodeInvalide, setPeriodeInvalide] = useState(false);
  const [statsError, setStatsError] = useState(false);
  const [historiqueError, setHistoriqueError] = useState(false);


  async function rattraper() {
    setRattrapageEnCours(true);
    setMessageProfil(null);
    try {
      const r = await reclamerRattrapage();
      setStats((prec) => (prec ? { ...prec, virtualBalance: r.virtualBalance, recoveryAvailable: false } : prec));
    } catch (e) {
      setMessageProfil(e instanceof Error ? e.message : 'Rattrapage impossible');
    } finally {
      setRattrapageEnCours(false);
    }
  }

  const statsRequestRef = useRef<AbortController | null>(null);
  const historyRequestRef = useRef<AbortController | null>(null);
  const dashboardRequestRef = useRef<AbortController | null>(null);

  const chargerStats = useCallback(() => {
    if (!profilId) return;
    statsRequestRef.current?.abort();
    const controller = new AbortController();
    statsRequestRef.current = controller;
    const chemin = userId ? `/stats/users/${userId}` : '/stats/me';
    setStatsError(false);
    api<Stats>(chemin, { signal: controller.signal })
      .then((donnees) => {
        if (!controller.signal.aborted) setStats(donnees);
      })
      .catch(() => {
        // L'échec est remonté dans l'interface, jamais dans la console :
        // le sujet impose une console de navigateur vierge.
        if (controller.signal.aborted) return;
        setStatsError(true);
      });
  }, [profilId, userId]);

  const chargerHistorique = useCallback(() => {
    if (!profilId) return;
    historyRequestRef.current?.abort();
    const controller = new AbortController();
    historyRequestRef.current = controller;
    const chemin = userId ? `/stats/users/${userId}/round-history` : '/stats/me/round-history';
    setHistoriqueError(false);
    api<MancheJouee[]>(chemin, { signal: controller.signal })
      .then((donnees) => {
        if (!controller.signal.aborted) setHistorique(donnees);
      })
      .catch(() => {
        if (controller.signal.aborted) return;
        setHistoriqueError(true);
      });
  }, [profilId, userId]);

  const chargerDashboard = useCallback(() => {
    if (!profilId || !estMonProfil) return;
    if (periode === 'custom') {
      if (!dateDebut || !dateFin) {
        setDashboard(null);
        setDashboardLoading(false);
        setDashboardError(false);
        setPeriodeInvalide(false);
        return;
      }
      // Un <input type="date"> émet une valeur à chaque date complète : en
      // saisissant l'année « 6666 » le navigateur produit d'abord 0006, 0066
      // puis 0666. Sans ce contrôle, chacune partait au serveur, qui refusait
      // l'intervalle inversé par un 400 — journalisé en rouge par Chrome à
      // chaque frappe. Les dates sont au format YYYY-MM-DD, donc comparables
      // directement en tant que chaînes.
      if (dateDebut > dateFin) {
        dashboardRequestRef.current?.abort();
        setDashboard(null);
        setDashboardLoading(false);
        setDashboardError(false);
        setPeriodeInvalide(true);
        return;
      }
    }
    setPeriodeInvalide(false);
    dashboardRequestRef.current?.abort();
    const controller = new AbortController();
    dashboardRequestRef.current = controller;
    setDashboardLoading(true);
    setDashboardError(false);
    statistiquesDashboard(periode, dateDebut, dateFin, controller.signal)
      .then((donnees) => {
        if (!controller.signal.aborted) setDashboard(donnees);
      })
      .catch(() => {
        if (controller.signal.aborted) return;
        setDashboardError(true);
      })
      .finally(() => {
        if (!controller.signal.aborted) setDashboardLoading(false);
      });
  }, [profilId, estMonProfil, periode, dateDebut, dateFin]);

  useEffect(() => {
    if (!profilId) return;
    chargerStats();
    chargerHistorique();
    chargerDashboard();
    return () => {
      statsRequestRef.current?.abort();
      historyRequestRef.current?.abort();
      dashboardRequestRef.current?.abort();
    };
  }, [profilId, chargerStats, chargerHistorique, chargerDashboard]);

  useEffect(() => {
    const invalider = (event: Event) => {
      const donnees = (event as CustomEvent<{ userId?: string }>).detail;
      if (donnees?.userId && donnees.userId !== profilId) return;
      chargerStats();
      if (estMonProfil) chargerDashboard();
    };
    window.addEventListener('factarena:stats-invalidate', invalider);
    return () => window.removeEventListener('factarena:stats-invalidate', invalider);
  }, [profilId, estMonProfil, chargerStats, chargerDashboard]);

  useEffect(() => {
    const invalider = (event: Event) => {
      const donnees = (event as CustomEvent<{ userId?: string }>).detail;
      if (donnees?.userId && donnees.userId !== profilId) return;
      chargerHistorique();
    };
    window.addEventListener('factarena:history-invalidate', invalider);
    return () => window.removeEventListener('factarena:history-invalidate', invalider);
  }, [profilId, chargerHistorique]);

  useEffect(() => {
    const mettreAJourSolde = (event: Event) => {
      const soldeMisAJour = (event as CustomEvent<{ userId?: string; virtualBalance?: number; recoveryAvailable?: boolean }>).detail;
      // Le nouveau solde est extrait dans une constante : le rétrécissement de
      // type d'un accès de propriété ne survit pas à l'entrée dans la closure
      // de setStats, celui d'un const local si.
      const nouveauSolde = soldeMisAJour?.virtualBalance;
      if (soldeMisAJour?.userId !== profilId || typeof nouveauSolde !== 'number') return;
      setStats((precedent) => precedent
        ? {
            ...precedent,
            virtualBalance: nouveauSolde,
            ...(typeof soldeMisAJour.recoveryAvailable === 'boolean'
              ? { recoveryAvailable: soldeMisAJour.recoveryAvailable }
              : {}),
          }
        : precedent);
    };
    window.addEventListener('factarena:balance-update', mettreAJourSolde);
    return () => window.removeEventListener('factarena:balance-update', mettreAJourSolde);
  }, [profilId]);

  useEffect(() => {
    const mettreAJourProfil = (event: Event) => {
      const profilMisAJour = (event as CustomEvent<{ id?: string; displayName?: string | null; avatarUrl?: string | null; achievements?: Array<{ achievementKey: string; unlockedAt: string }> }>).detail;
      if (!profilMisAJour?.id) return;
      setStats((precedent) => precedent && precedent.user.id === profilMisAJour.id
        ? { ...precedent, user: { ...precedent.user, displayName: profilMisAJour.displayName ?? precedent.user.displayName, avatarUrl: profilMisAJour.avatarUrl ?? precedent.user.avatarUrl, achievements: profilMisAJour.achievements ?? precedent.user.achievements } }
        : precedent);
    };
    window.addEventListener('factarena:profile-update', mettreAJourProfil);
    return () => window.removeEventListener('factarena:profile-update', mettreAJourProfil);
  }, []);

  const nomDe = (profil?: ProfilJoueur | null, secours?: string) =>
    profil?.displayName || profil?.email?.split('@')[0]?.slice(0, 30) || secours || 'joueur';
  const pseudo = nomDe(
    stats?.user,
    estMonProfil ? utilisateur?.displayName || utilisateur?.email?.split('@')[0]?.slice(0, 30) : undefined,
  );
  const succesDebloques = new Set((stats?.user.achievements ?? []).map((succes) => succes.achievementKey));


  async function enregistrerPseudo(event: React.FormEvent) {
    event.preventDefault();
    setMessageProfil(null);
    const valeur = nouveauPseudo.trim();
    if (valeur.length < 2 || valeur.length > 30) {
      setMessageProfil('Le pseudo doit faire entre 2 et 30 caractères.');
      return;
    }
    try {

      const joueursCorrespondants = await chercherJoueurs(valeur);
      const pseudoDejaUtilise = joueursCorrespondants.some((joueur) =>
        joueur.displayName?.toLocaleLowerCase() === valeur.toLocaleLowerCase(),
      );
      if (pseudoDejaUtilise) {
        setMessageProfil('Pseudo déjà utilisé par un autre joueur.');
        return;
      }
      await modifierProfil({ displayName: valeur });

      setStats((precedent) => precedent
        ? { ...precedent, user: { ...precedent.user, displayName: valeur } }
        : precedent);

      await rafraichir();
      setEdition(false);
    } catch (e) {
      setMessageProfil(e instanceof Error ? e.message : 'Modification impossible');
    }
  }

  async function choisirAvatar(event: React.ChangeEvent<HTMLInputElement>) {
    const fichier = event.target.files?.[0];
    if (!fichier) return;
    setMessageProfil(null);
    try {
      if (!['image/png', 'image/jpeg', 'image/webp'].includes(fichier.type)) {
        setMessageProfil('Format accepté : PNG, JPEG/JPG ou WebP.');
        return;
      }
      if (fichier.size > 5 * 1024 * 1024) {
        setMessageProfil('Le fichier avatar ne doit pas dépasser 5 Mo.');
        return;
      }
      const url = URL.createObjectURL(fichier);
      const dimensions = await new Promise<{ width: number; height: number }>((resolve, reject) => {
        const image = new Image();
        image.onload = () => resolve({ width: image.naturalWidth, height: image.naturalHeight });
        image.onerror = () => reject(new Error('Image invalide.'));
        image.src = url;
      }).finally(() => URL.revokeObjectURL(url));
      if (dimensions.width < 1 || dimensions.height < 1 || dimensions.width > 2048 || dimensions.height > 2048 || dimensions.width * dimensions.height > 4_000_000) {
        setMessageProfil('Les dimensions de l’avatar ne doivent pas dépasser 2048 × 2048 pixels.');
        return;
      }
      await envoyerAvatar(fichier);
      await rafraichir();
    } catch (e) {
      setMessageProfil(e instanceof Error ? e.message : 'Envoi impossible');
    } finally {

      event.target.value = '';
    }
  }

  return (
    <section className="p-4 sm:p-8 bg-noir-krystal w-full min-h-full text-white">
      <div className="max-w-5xl mx-auto">
        <header className="flex flex-col items-center text-center">
          <h1 className="text-2xl sm:text-3xl font-semibold break-all">{pseudo}</h1>
          <div className="mt-5">
            {stats?.user.avatarUrl ? (
              <img src={stats.user.avatarUrl} alt={`Avatar de ${pseudo}`} className="w-56 h-56 sm:w-64 sm:h-64 rounded-full object-cover" />
            ) : (
              <span className="w-56 h-56 sm:w-64 sm:h-64 rounded-full bg-grisclair-krystal flex items-center justify-center text-7xl sm:text-8xl font-semibold">
                {pseudo.charAt(0).toUpperCase()}
              </span>
            )}
          </div>
          {estMonProfil && <div className="mt-5 flex items-stretch justify-center">
            <div className="flex items-stretch justify-center w-full sm:w-[220px] min-h-[74px] bg-gris-krystal rounded-l-xl">
              {estMonProfil && edition ? (
                <form onSubmit={enregistrerPseudo} className="flex flex-col gap-2 items-center w-full">
                  <label htmlFor="pseudo" className="sr-only">Nouveau pseudo</label>
                  <input id="pseudo" type="text" value={nouveauPseudo} onChange={(e) => setNouveauPseudo(e.target.value)} minLength={2} maxLength={30} required className="bg-noir-krystal rounded-lg px-3 py-2 text-sm outline-none w-full" />
                  <div className="flex gap-3 text-xs"><button type="submit" className="text-bleu-l hover:underline">ok</button><button type="button" onClick={() => { setEdition(false); setMessageProfil(null); }} className="text-white/40 hover:text-white">annuler</button></div>
                </form>
              ) : (
                <button type="button" onClick={() => { setNouveauPseudo(pseudo); setEdition(true); }} disabled={!estMonProfil} className="flex w-full min-h-[74px] items-center justify-center rounded-l-xl px-5 py-3 text-sm hover:bg-grisclair-krystal disabled:cursor-default disabled:text-white/70">Modifier le pseudo</button>
              )}
            </div>
            <label className="flex flex-col items-center justify-center px-5 py-3 w-full sm:w-[220px] min-h-[74px] bg-gris-krystal rounded-r-xl border-l border-white/10 text-sm cursor-pointer hover:bg-grisclair-krystal">
              <span>Changer d’avatar</span>
              <span className="text-xs text-white/45 mt-1">(5 Mo max, PNG/JPEG/WebP<br />2048 × 2048 px)</span>
              <input id="avatar" name="avatar" type="file" accept="image/png,image/jpeg,image/webp" className="hidden" disabled={!estMonProfil} onChange={choisirAvatar} />
            </label>
          </div>}
          {messageProfil && <p className="text-red-l text-xs mt-2">{messageProfil}</p>}
        </header>

        {statsError && <p className="text-red-l text-sm mt-6 text-center">Statistiques du profil momentanément indisponibles.</p>}

        <div className="grid md:grid-cols-2 gap-4 mt-10">
          <div className="bg-gris-krystal rounded-xl p-6 text-center">
            <p className="text-sm text-white/50">Taux de réussite</p>
            <p className="text-4xl font-semibold mt-2">{stats ? `${Math.round(stats.winRate * 100)} %` : '—'}</p>
            <p className="mt-3 text-sm font-medium"><span className="text-emerald-500">{stats?.victories ?? '—'} Victoires</span><span className="text-white/35 mx-2">/</span><span className="text-red-500">{stats?.defeats ?? '—'} Défaites</span></p>
          </div>
          <div className="bg-gris-krystal rounded-xl p-6 text-center relative">
            <p className="text-sm text-white/50">Crédits total</p>
            <p className="text-4xl font-semibold mt-2">{stats ? stats.virtualBalance : '—'} cr</p>
            <p className="mt-3 text-sm text-white/55">Sur {stats?.roundsPlayed ?? '—'} manches jouées</p>
            {estMonProfil && stats?.recoveryAvailable && stats.virtualBalance === 0 && (
              <button type="button" onClick={rattraper} disabled={rattrapageEnCours} className="button-blue-l px-3 py-2 text-xs mt-4 md:absolute md:right-4 md:top-1/2 md:-translate-y-1/2 disabled:opacity-50">{rattrapageEnCours ? '...' : 'Réclamer 200 crédits'}</button>
            )}
          </div>
        </div>

        <div className="mt-8 bg-gris-krystal rounded-xl p-4 sm:p-6">
          <h2 className="text-xl font-semibold">Succès</h2>
          <div className="mt-4 grid gap-2 sm:grid-cols-2">
            {ACHIEVEMENTS.map((succes) => {
              const debloque = succesDebloques.has(succes.key);
              return (
                <div
                  key={succes.key}
                  className={`rounded-xl border p-3 transition-colors ${debloque ? 'border-amber-300/50 bg-amber-300/10 text-amber-200' : 'border-white/10 bg-noir-krystal/50 text-white/35 grayscale'}`}
                >
                  <p className="font-semibold">{succes.name}</p>
                  <p className="mt-1 text-xs">{succes.description}</p>
                </div>
              );
            })}
          </div>
        </div>

        {estMonProfil && (
          <div className="mt-8 bg-gris-krystal rounded-xl p-4 sm:p-6">
            <div className="flex flex-col lg:flex-row lg:items-center justify-between gap-4">
              <h2 className="text-xl font-semibold">Statistiques personnelles</h2>
              <div className="flex flex-wrap gap-2">{dashboard && <><button type="button" onClick={() => exportPersonalCsv(dashboard)} className="button-blue-d px-3 py-2 text-xs">CSV</button><button type="button" onClick={() => exportPersonalPdf(dashboard)} className="button-blue-d px-3 py-2 text-xs">PDF</button></>}</div>
            </div>
            <div className="mt-4"><PeriodSelector value={periode} onChange={setPeriode} from={dateDebut} to={dateFin} onFromChange={setDateDebut} onToChange={setDateFin} /></div>
            {periodeInvalide ? <p className="py-10 text-center text-red-l text-sm">La date de début doit précéder la date de fin.</p> : dashboardLoading ? <p className="py-10 text-center text-white/40 text-sm">Chargement des statistiques…</p> : dashboardError ? <p className="py-10 text-center text-red-l text-sm">Statistiques momentanément indisponibles. La partie et le profil restent fonctionnels.</p> : periode === 'custom' && (!dateDebut || !dateFin) ? <EmptyStats text="Choisis une date de début et une date de fin." /> : !dashboard ? <EmptyStats text="Les statistiques apparaîtront après les premières données." /> : (
              <>
                <div className="grid lg:grid-cols-2 gap-6 mt-6 min-h-[420px]">
                  <div className="bg-noir-krystal rounded-xl p-4 flex flex-col justify-center min-w-0"><h3 className="font-semibold text-center">Précision par thème</h3><AccuracyRadar categories={dashboard.accuracy.byCategory} /></div>
                  <div className="bg-noir-krystal rounded-xl p-4 flex flex-col justify-center min-w-0"><h3 className="font-semibold text-center">Meilleurs gains / pertes</h3><GainLossPie bestGain={dashboard.bestGain} worstLoss={dashboard.worstLoss} /></div>
                </div>
                <div className="mt-6 bg-noir-krystal rounded-xl p-4"><h3 className="font-semibold">Solde des crédits par manche</h3><BalanceEvolution points={dashboard.roundBalanceHistory} /></div>
              </>
            )}
          </div>
        )}

      {}
      <div className="lg:col-span-5 mt-6">
        <h1 className="bg-grisclair-krystal text-[20px] px-8 py-2 w-full lg:w-[18%] text-white font-semibold rounded-xl">
          Historique
        </h1>
        <div className="bg-gris-krystal rounded-xl p-4 sm:p-6 mt-2">
          {historiqueError ? (
            <p className="text-red-l text-sm">
              Historique momentanément indisponible. La partie et le profil restent fonctionnels.
            </p>
          ) : historique.length === 0 ? (
            <p className="text-white/30 text-sm">
              Aucune manche terminée pour l'instant.
            </p>
          ) : (
            <ul className="flex flex-col gap-2">
              {historique.map((manche) => {

                const adversaires = manche.round.bets
                  .filter((b) => b.user?.id !== stats?.user.id)
                  .map((b) => nomDe(b.user));
                const gagne = manche.result === 'WIN';
                const annulee = manche.result === 'CANCELLED';

                const net = (manche.payout ?? 0) - manche.stake;
                const date = new Date(manche.round.revealedAt ?? manche.round.createdAt);
                return (
                  <li
                    key={manche.id}
                    className="bg-noir-krystal rounded-xl p-3 flex flex-wrap items-center gap-x-4 gap-y-1 text-white"
                  >
                    <span
                      className={`text-xs font-semibold px-2 py-0.5 rounded-full shrink-0 ${
                        annulee ? 'bg-white/10 text-white/50' : gagne ? 'bg-emerald-400/15 text-emerald-400' : 'bg-red-400/15 text-red-400'
                      }`}
                    >
                      {annulee ? 'annulée' : gagne ? 'gagnée' : 'perdue'}
                    </span>
                    <span className="text-sm text-white/50 shrink-0">
                      {date.toLocaleDateString('fr-FR')} à {date.toLocaleTimeString('fr-FR', { hour: '2-digit', minute: '2-digit' })}
                    </span>
                    <span className="text-sm truncate min-w-0 flex-1">
                      {manche.round.room?.name ?? 'salon supprimé'}
                      {adversaires.length > 0 && (
                        <span className="text-white/40"> · contre {adversaires.join(', ')}</span>
                      )}
                    </span>
                    <span className="text-sm text-white/40 shrink-0">mise {manche.stake}</span>
                    <span
                      className={`text-sm font-semibold shrink-0 ${net >= 0 ? 'text-emerald-400' : 'text-red-400'}`}
                    >
                      {net >= 0 ? '+' : ''}{net} cr
                    </span>
                  </li>
                );
              })}
            </ul>
          )}
        </div>
      </div>

      </div>
    </section>
  );
}
