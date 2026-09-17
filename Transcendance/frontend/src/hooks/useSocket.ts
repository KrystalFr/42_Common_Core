
import { useCallback, useEffect, useRef, useState } from 'react';
import { io, type Socket } from 'socket.io-client';
import { getToken } from '../api/client';

export interface EtatSalon {
  room: { id: string; name: string; status: string; createdBy: string; lobbyExpiresAt?: string | null; game?: { id: string; status: string; currentRoundIndex: number; startedAt: string; finishedAt?: string | null } | null };
  members: unknown[];
  round: Record<string, unknown> | null;
  pot: number;
  betCount: number;
  multipliers?: { FACT: number | null; FAKE: number | null };
  serverTime: string;
}

export function useSocket(roomId: string | null) {
  const socketRef = useRef<Socket | null>(null);
  const [connecte, setConnecte] = useState(false);
  const [etat, setEtat] = useState<EtatSalon | null>(null);
  const [erreur, setErreur] = useState<string | null>(null);

  const decalageRef = useRef(0);

  useEffect(() => {
    if (!roomId) return;
    const token = getToken();

    if (!token) return;


    const socket = io({
      path: '/socket.io',
      transports: ['websocket'],
      auth: { token, roomId },
    });
    socketRef.current = socket;

    socket.on('connect', () => {
      setConnecte(true);
    });
    socket.on('disconnect', () => setConnecte(false));
    socket.on('room:error', (donnees: { message?: string }) => {
      setErreur(donnees?.message ?? 'Impossible de rejoindre la room.');
    });

    const majEtat = (nouvel: EtatSalon) => {
      if (!nouvel?.room) return;
      if (nouvel?.serverTime) {
        decalageRef.current = new Date(nouvel.serverTime).getTime() - Date.now();
      }
      setEtat(nouvel);
    };

    const EVENEMENTS_ETAT = [
      'room:state', 'room:joined', 'room:reconnected', 'room:ready', 'room:member_joined', 'room:member_left', 'room:kicked', 'room:expired', 'room:destroyed',
      'game:started', 'game:finished', 'round:state', 'round:started', 'round:revealed', 'answer:submitted', 'bet:confirmed',
    ];
    EVENEMENTS_ETAT.forEach((evenement) => socket.on(evenement, majEtat));


    socket.on('connect_error', () => setConnecte(false));

    return () => {
      socket.removeAllListeners();
      socket.disconnect();
      socketRef.current = null;
    };
  }, [roomId]);


  const emettre = useCallback((evenement: string, donnees: unknown) => {
    socketRef.current?.emit(evenement, donnees);
  }, []);

  const demander = useCallback(<T,>(
    evenement: string,
    donnees: unknown,
    options?: { reconnect?: boolean },
  ): Promise<T> => {
    const envoyer = (socket: Socket): Promise<T> => new Promise<T>((resolve, reject) => {
      socket.timeout(6000).emit(evenement, donnees, (erreur: Error | null, reponse: T) => {
        if (erreur) reject(erreur);
        else {
          const resultat = reponse as T & { ok?: boolean; message?: string };
          if (resultat && resultat.ok === false) reject(new Error(resultat.message ?? 'Action impossible'));
          else resolve(reponse);
        }
      });
    });

    let socket = socketRef.current;
    if (socket?.connected) return envoyer(socket);

    if (!options?.reconnect) {
      return Promise.reject(new Error('Connexion au salon indisponible'));
    }

    return new Promise<T>((resolve, reject) => {
      let termine = false;
      let timer = 0;
      let attenteSocket = 0;
      let ecouteSocket: Socket | null = null;

      const nettoyer = () => {
        if (timer) window.clearTimeout(timer);
        if (attenteSocket) window.clearInterval(attenteSocket);
        if (ecouteSocket) ecouteSocket.off('connect', onConnect);
      };

      const echouer = () => {
        if (termine) return;
        termine = true;
        nettoyer();
        reject(new Error('Connexion au salon indisponible'));
      };

      const onConnect = () => {
        if (termine) return;
        socket = socketRef.current;
        if (!socket?.connected) return;
        termine = true;
        nettoyer();
        envoyer(socket).then(resolve, reject);
      };

      const brancher = () => {
        if (termine) return;
        socket = socketRef.current;
        if (!socket) return;
        ecouteSocket = socket;
        socket.once('connect', onConnect);
        if (socket.connected) {
          onConnect();
          return;
        }
        socket.connect();
      };

      brancher();
      if (!ecouteSocket) {
        attenteSocket = window.setInterval(brancher, 25);
      }
      timer = window.setTimeout(echouer, 6000);
    });
  }, []);


  // `never` en paramètre rendait tout rappel incompatible avec socket.on.
  // Un générique laisse l'appelant typer sa charge utile, et la même référence
  // de gestionnaire est utilisée pour on/off afin que le nettoyage fonctionne.
  const ecouter = useCallback(<T = unknown>(evenement: string, rappel: (donnees: T) => void) => {
    const socket = socketRef.current;
    if (!socket) return () => {};
    const gestionnaire = rappel as (...args: unknown[]) => void;
    socket.on(evenement, gestionnaire);
    return () => {
      socket.off(evenement, gestionnaire);
    };
  }, []);


  const deconnecterVolontairement = useCallback(() => {
    const socket = socketRef.current;
    if (!socket) return;
    socket.removeAllListeners();
    socket.disconnect();
    socketRef.current = null;
    setConnecte(false);
  }, []);

  const maintenantServeur = useCallback(() => Date.now() + decalageRef.current, []);

  return { connecte, etat, erreur, emettre, demander, ecouter, deconnecterVolontairement, maintenantServeur };
}
