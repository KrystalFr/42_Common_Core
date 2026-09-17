
import { useCallback, useEffect, useState } from 'react';
import { api } from '../api/client';
import { useAuth } from '../auth/AuthContext';

interface JoueurClassement {
  rank: number;
  user: { id: string; displayName: string | null; email: string };
  netGain: number;
  virtualBalance: number;
}

interface Salon {
  id: string;
  name: string;
  status: string;
  members?: Array<{ userId: string; ready: boolean }>;
}


export default function Play() {
  const { utilisateur } = useAuth();
  const [salons, setSalons] = useState<Salon[]>([]);
  const [chargement, setChargement] = useState(true);
  const [erreur, setErreur] = useState<string | null>(null);
  const [creation, setCreation] = useState(false);
  const [roomPleine, setRoomPleine] = useState<string | null>(null);
  const [classement, setClassement] = useState<JoueurClassement[]>([]);

  async function rejoindreSalon(salon: Salon) {
    setErreur(null);
    setRoomPleine(null);
    try {
      const membres = await api<Array<{ id: string }>>(`/rooms/${salon.id}/members`);
      const dejaDansLaRoom = membres.some((membre) => membre.id === utilisateur?.id);
      if (membres.length >= 5 && !dejaDansLaRoom) {
        setRoomPleine(salon.id);
        return;
      }
      sessionStorage.setItem('factarena:room-loading-in-progress', '1');
      window.location.replace(`/room/${salon.id}`);
    } catch (e) {
      setErreur(e instanceof Error ? e.message : 'Impossible de rejoindre la room');
    }
  }

  const charger = useCallback(() => {
    setChargement(true);
    api<Salon[]>('/rooms')
      .then((liste) => { setSalons(liste); setErreur(null); })
      .catch((e) => setErreur(e instanceof Error ? e.message : 'Chargement impossible'))
      .finally(() => setChargement(false));
  }, []);

  useEffect(charger, [charger]);

  useEffect(() => {
    const chargerClassement = () => {
      api<JoueurClassement[]>('/stats/leaderboard')
        .then((liste) => setClassement(liste.slice(0, 3)))
        .catch(() => {  });
    };
    chargerClassement();
    const invalider = () => chargerClassement();
    window.addEventListener('factarena:leaderboard-invalidate', invalider);
    return () => window.removeEventListener('factarena:leaderboard-invalidate', invalider);
  }, []);

  useEffect(() => {
    const mettreAJourSalons = () => charger();
    window.addEventListener('factarena:rooms-update', mettreAJourSalons);
    return () => window.removeEventListener('factarena:rooms-update', mettreAJourSalons);
  }, [charger]);

  async function creerSalon() {
    setCreation(true);
    sessionStorage.setItem('factarena:room-creation-in-progress', '1');
    window.dispatchEvent(new Event('factarena:room-creation-state'));
    setErreur(null);
    try {

      const salon = await api<Salon>('/rooms', {
        method: 'POST',
        body: { name: `Salon ${new Date().toLocaleTimeString('fr-FR')}` },
      });
      window.location.replace(`/room/${salon.id}`);
    } catch (e) {
      sessionStorage.removeItem('factarena:room-creation-in-progress');
      window.dispatchEvent(new Event('factarena:room-creation-state'));
      setErreur(e instanceof Error ? e.message : 'Création impossible');
    } finally {
      setCreation(false);
    }
  }

  return (
    <section className="grid gap-4 grid-cols-1 lg:grid-cols-2 p-4 sm:p-8 bg-noir-krystal w-full min-h-full">
      <div className="lg:col-span-1 lg:col-start-1">
        <h1 className="bg-grisclair-krystal text-[20px] px-8 py-2 w-full lg:w-[46%] text-white font-semibold rounded-xl">
          Rejoindre une room
        </h1>

        <div className="p-4 sm:p-8 gap-2 bg-gris-krystal min-h-[300px] sm:min-h-[90%] rounded-xl flex flex-col gap-3">
          {chargement && <p className="text-white">Chargement des salons...</p>}

          {!chargement && salons.length === 0 && (
            <p className="text-gray-300">Aucun salon pour le moment. Crée le premier.</p>
          )}

          {salons.map((salon) => (
            <button
              key={salon.id}
              type="button"
              onClick={() => rejoindreSalon(salon)}
              className="bg-noir-krystal rounded-xl text-white px-4 py-3 text-left hover:bg-grisclair-krystal transition-colors"
            >
              <span className="font-semibold">{salon.name}</span>
              <span className="text-sm text-gray-400 ml-3">{salon.members?.length ?? 0}/5 joueurs</span>
              {roomPleine === salon.id && (
                <span role="alert" className="block text-red-l text-sm mt-1">
                  La room est pleine (5 joueurs maximum).
                </span>
              )}
            </button>
          ))}

          {}
          {erreur && <p role="alert" className="text-red-l text-sm">{erreur}</p>}
        </div>
      </div>

      <div className="flex flex-col gap-4 lg:col-start-2 sm:p-8">
        <button
          type="button"
          onClick={creerSalon}
          disabled={creation}
          className="flex items-center justify-center px-6 sm:px-10 py-6 sm:py-10 button-blue-d text-xl disabled:opacity-50">
		  <img src="/images/blue.gif" alt="FactArena" className="h-20 lg:h-36 w-auto object-contain shrink-0"/>
          {creation ? 'Création...' : 'Créer une room'}
        </button>

        <button type="button" onClick={charger} className="px-6 py-2 button-blue-l">
          Rafraîchir la liste
        </button>

        <div className="mt-1">
          <h2 className="bg-grisclair-krystal text-[20px] px-8 py-2 w-full text-white font-semibold rounded-xl">
            Meilleurs joueurs du classement
          </h2>
          <div className="grid grid-cols-3 grid-rows-4 p-4 sm:p-8 gap-2 bg-gris-krystal min-h-[320px] rounded-xl">
            {['col-start-1 col-end-1 row-start-1 row-span-4', 'col-start-2 col-end-2 row-start-2 row-span-3', 'col-start-3 col-end-3 row-start-3 row-span-2'].map((position, index) => {
              const joueur = classement[index];
              return (
                <div key={position} className={`${position} min-w-0 max-w-full overflow-hidden bg-noir-krystal rounded-xl text-white flex flex-col items-center justify-center p-3 gap-1`}>
                  {joueur && <>
					<img
					src={`/images/${index + 1}.png`}
					alt=""
					className="w-16 h-16 object-contain mb-2"
					/>
                    <span className="text-white/40 text-xs">#{joueur.rank}</span>
                    <span className="min-w-0 max-w-full font-semibold text-sm text-center break-all">{joueur.user.displayName || joueur.user.email.split('@')[0]}</span>
                    <span className="text-white/70 text-xs">{joueur.virtualBalance} cr</span>
                  </>}
                </div>
              );
            })}
          </div>
        </div>
      </div>
    </section>
  );
}
