import {
  createContext,
  useCallback,
  useContext,
  useEffect,
  useMemo,
  useRef,
  useState,
  type ReactNode,
} from 'react';
import { useLocation, useNavigate } from 'react-router';
import {
  api,
  clearToken,
  SESSION_EXPIREE,
  connexion as apiConnexion,
  demandesAmis,
  getToken,
  inscription as apiInscription,
  profil,
  setToken,
  tokenExpire,
  type Utilisateur,
} from '../api/client';
import { io, type Socket } from 'socket.io-client';

interface ContexteAuth {
  utilisateur: Utilisateur | null;

  chargement: boolean;
  seConnecter: (email: string, motDePasse: string) => Promise<void>;
  sInscrire: (email: string, motDePasse: string, pseudo: string) => Promise<void>;
  seDeconnecter: () => void;

  rafraichir: () => Promise<void>;
  notificationsAmis: number;
}

const Contexte = createContext<ContexteAuth | null>(null);

export function AuthProvider({ children }: { children: ReactNode }) {
  const [utilisateur, setUtilisateur] = useState<Utilisateur | null>(null);
  const [chargement, setChargement] = useState(true);
  const [notificationsAmis, setNotificationsAmis] = useState(0);
  const demandesAmisVues = useRef(false);
  const socketGlobaleRef = useRef<Socket | null>(null);
  const recuperationTraiteeRef = useRef(false);
  const navigate = useNavigate();
  const emplacement = useLocation();


  useEffect(() => {
    const token = getToken();

    if (!token || tokenExpire(token)) {
      if (token) clearToken();
      setChargement(false);
      return;
    }
    let annule = false;
    profil()
      .then((u) => {
        if (annule) return;
        if (u) {
          setUtilisateur(u);
        } else {
          clearToken();
          setUtilisateur(null);
        }
      })
      .catch(() => {

        clearToken();
      })
      .finally(() => {
        if (!annule) setChargement(false);
      });
    return () => {
      annule = true;
    };
  }, []);

  useEffect(() => {
    if (chargement || !utilisateur?.activeRoomId || recuperationTraiteeRef.current) return;
    const roomId = utilisateur.activeRoomId;
    if (emplacement.pathname === `/room/${roomId}`) {
      recuperationTraiteeRef.current = true;
      return;
    }
    recuperationTraiteeRef.current = true;
    let actif = true;
    api<any>(`/rooms/${roomId}`)
      .then((room) => {
        if (!actif || !room || !['WAITING', 'IN_PROGRESS'].includes(room.status)) return;
        const membre = Array.isArray(room.members) && room.members.some((member: any) => member.userId === utilisateur.id);
        if (membre) navigate(`/room/${roomId}`, { replace: true });
      })
      .catch(() => {  });
    return () => { actif = false; };
  }, [chargement, utilisateur?.activeRoomId, utilisateur?.id, emplacement.pathname, navigate]);


  useEffect(() => {
    if (!utilisateur) return;
    demandesAmisVues.current = false;
    demandesAmis().then((demandes) => {
      if (!demandesAmisVues.current) setNotificationsAmis(demandes.received.length);
    }).catch(() => undefined);
    const token = getToken();
    if (!token) return;
    const socket = io({ path: '/socket.io', transports: ['websocket'], auth: { token } });
    socketGlobaleRef.current = socket;
    const profilMisAJour = (profilUtilisateur: { id?: string; displayName?: string | null; avatarUrl?: string | null; achievements?: Array<{ achievementKey: string; unlockedAt: string }> }) => {
      if (!profilUtilisateur?.id) return;
      if (profilUtilisateur.id === utilisateur.id) {
        setUtilisateur((precedent) => precedent ? {
          ...precedent,
          displayName: profilUtilisateur.displayName ?? precedent.displayName,
          avatarUrl: profilUtilisateur.avatarUrl ?? precedent.avatarUrl,
        } : precedent);
      }
      window.dispatchEvent(new CustomEvent('factarena:profile-update', { detail: profilUtilisateur }));
    };
    const relayer = (evenement: string, donnees: unknown) => {
      window.dispatchEvent(new CustomEvent(`factarena:${evenement}`, { detail: donnees }));
    };
    socket.on('profile:update', profilMisAJour);
    socket.on('stats:invalidate', (donnees: unknown) => relayer('stats-invalidate', donnees));
    socket.on('leaderboard:invalidate', () => relayer('leaderboard-invalidate', undefined));
    socket.on('history:invalidate', (donnees: unknown) => relayer('history-invalidate', donnees));
    socket.on('presence:update', (donnees: unknown) => relayer('presence-update', donnees));
    socket.on('rooms:update', () => relayer('rooms-update', undefined));
    socket.on('balance:update', (donnees: unknown) => relayer('balance-update', donnees));
    socket.on('friend:request-received', (donnees: { requestId?: string }) => {
      window.dispatchEvent(new CustomEvent('factarena:friend-update', { detail: { type: 'request_received', ...donnees } }));
      setNotificationsAmis((nombre) => nombre + 1);
    });
    socket.on('friend:request-accepted', (donnees: { requestId?: string }) => {
      window.dispatchEvent(new CustomEvent('factarena:friend-update', { detail: { type: 'request_accepted', ...donnees } }));
    });
    socket.on('friend:request-rejected', (donnees: { requestId?: string }) => {
      window.dispatchEvent(new CustomEvent('factarena:friend-update', { detail: { type: 'request_rejected', ...donnees } }));
    });
    socket.on('friend:removed', (donnees: { friendId?: string }) => {
      window.dispatchEvent(new CustomEvent('factarena:friend-update', { detail: { type: 'friend_removed', ...donnees } }));
    });
    return () => {
      socket.removeAllListeners();
      socket.disconnect();
      if (socketGlobaleRef.current === socket) socketGlobaleRef.current = null;
    };
  }, [utilisateur?.id]);

  useEffect(() => {
    const voirAmis = () => {
      demandesAmisVues.current = true;
      setNotificationsAmis(0);
    };
    window.addEventListener('factarena:friends-viewed', voirAmis);
    return () => window.removeEventListener('factarena:friends-viewed', voirAmis);
  }, []);


  useEffect(() => {
    const surExpiration = () => setUtilisateur(null);
    window.addEventListener(SESSION_EXPIREE, surExpiration);
    return () => window.removeEventListener(SESSION_EXPIREE, surExpiration);
  }, []);

  const seConnecter = useCallback(async (email: string, motDePasse: string) => {
    const reponse = await apiConnexion(email, motDePasse);
    // Le test `in` seul suffit à écarter la branche d'échec : y ajouter
    // `&& reponse.loginFailed` empêchait TypeScript de réduire l'union, et
    // `reponse.accessToken` était alors considéré comme inexistant.
    if ('loginFailed' in reponse) {
      throw new Error(reponse.message);
    }
    setToken(reponse.accessToken);
    recuperationTraiteeRef.current = false;
    setUtilisateur(reponse.user);
  }, []);

  const sInscrire = useCallback(async (email: string, motDePasse: string, pseudo: string) => {
    const reponse = await apiInscription(email, motDePasse, pseudo);
    if ('alreadyExists' in reponse) {
      throw new Error(reponse.message);
    }
    if ('pseudoAlreadyExists' in reponse) {
      throw new Error(reponse.message);
    }
    setToken(reponse.accessToken);
    recuperationTraiteeRef.current = false;
    setUtilisateur(reponse.user);
  }, []);

  const seDeconnecter = useCallback(() => {
    const socket = socketGlobaleRef.current;
    if (socket) {
      socket.removeAllListeners();
      socket.disconnect();
      socketGlobaleRef.current = null;
    }
    clearToken();
    demandesAmisVues.current = true;
    setNotificationsAmis(0);
    setUtilisateur(null);
  }, []);

  const rafraichir = useCallback(async () => {
    try {
      const profilActuel = await profil();
      if (profilActuel) {
        setUtilisateur(profilActuel);
      } else {
        clearToken();
        setUtilisateur(null);
      }
    } catch {

    }
  }, []);

  const valeur = useMemo(
    () => ({ utilisateur, chargement, seConnecter, sInscrire, seDeconnecter, rafraichir, notificationsAmis }),
    [utilisateur, chargement, seConnecter, sInscrire, seDeconnecter, rafraichir, notificationsAmis],
  );

  return <Contexte.Provider value={valeur}>{children}</Contexte.Provider>;
}

export function useAuth(): ContexteAuth {
  const contexte = useContext(Contexte);
  if (!contexte) {
    throw new Error('useAuth doit être utilisé à l\'intérieur d\'un <AuthProvider>');
  }
  return contexte;
}
