import { Outlet, Link, useLocation, useNavigate } from 'react-router';
import { useCallback, useEffect, useRef, useState } from 'react';
import Footer from './Footer';
import { useAuth } from '../auth/AuthContext';


export default function Layout() {
  const navigate = useNavigate();
  const location = useLocation();
  const quitterRoomRef = useRef<(() => Promise<void>) | null>(null);
  const registerRoomLeave = useCallback((handler: (() => Promise<void>) | null) => {
    quitterRoomRef.current = handler;
  }, []);
  const { utilisateur, seDeconnecter, notificationsAmis } = useAuth();
  const [logoutEnCours, setLogoutEnCours] = useState(false);
  const [creationRoomEnCours, setCreationRoomEnCours] = useState(() => sessionStorage.getItem('factarena:room-creation-in-progress') === '1');
  const [chargementRoomEnCours, setChargementRoomEnCours] = useState(() => sessionStorage.getItem('factarena:room-loading-in-progress') === '1');
  const logoutEnCoursRef = useRef(false);
  const pseudo = utilisateur?.displayName || utilisateur?.email.split('@')[0].slice(0, 30);
  const dansUneRoom = /^\/room\/[^/]+$/.test(location.pathname);
  const boutonsBloques = logoutEnCours || creationRoomEnCours || (dansUneRoom && chargementRoomEnCours);
  useEffect(() => {
    const synchroniserCreationRoom = () => {
      setCreationRoomEnCours(sessionStorage.getItem('factarena:room-creation-in-progress') === '1');
      setChargementRoomEnCours(sessionStorage.getItem('factarena:room-loading-in-progress') === '1');
    };
    window.addEventListener('factarena:room-creation-state', synchroniserCreationRoom);
    return () => window.removeEventListener('factarena:room-creation-state', synchroniserCreationRoom);
  }, []);

  async function quitterLaRoom() {
    if (logoutEnCoursRef.current) return;
    logoutEnCoursRef.current = true;
    setLogoutEnCours(true);
    try {
      const leave = quitterRoomRef.current;
      if (leave) await leave();
      logoutEnCoursRef.current = false;
      setLogoutEnCours(false);
      navigate('/play');
    } catch {
      logoutEnCoursRef.current = false;
      setLogoutEnCours(false);
    }
  }

  async function handleLogout() {
    if (logoutEnCoursRef.current) return;
    logoutEnCoursRef.current = true;
    setLogoutEnCours(true);

    try {
      const leave = dansUneRoom ? quitterRoomRef.current : null;
      if (leave) {
        try {
          await leave();
        } catch {
        }
      }
    } finally {
      seDeconnecter();
      logoutEnCoursRef.current = false;
      setLogoutEnCours(false);
      navigate('/');
    }
  }

  return (
    <div className="min-h-screen flex flex-col">
      <header className="p-4 sm:p-8 relative bg-noir-krystal">
        <nav className="font-semibold text-white flex flex-wrap gap-2 sm:gap-4 items-center">
          {dansUneRoom ? (
            <button
              type="button"
              className="text-base sm:text-lg button-red-d px-3 sm:px-5 py-1 disabled:opacity-50 disabled:cursor-not-allowed"
              onClick={() => { void quitterLaRoom(); }}
              disabled={boutonsBloques}
            >
              Quitter la partie
            </button>
          ) : (
			<div className ="flex items-center gap-1 sm:gap-2">
				<img src="/images/home.png" alt="FactArena" className="h-8 sm:h-9 lg:h-20 w-auto object-contain shrink-0" />
            	<Link to="/" className="text-base sm:text-lg button-blue-d px-3 sm:px-5 py-1">Accueil</Link>
			</div>
          )}

          {!dansUneRoom && !utilisateur && (
			<div className ="ml-auto flex items-center gap-1 sm:gap-2">
				<Link to="/login" className="text-base sm:text-lg button-blue-d px-3 sm:px-5 py-1">
              	Connexion
            	</Link>
				<img src="/images/login.png" alt="" className="h-8 sm:h-9 lg:h-20 w-auto object-contain shrink-0"/>
			</div>
          )}

          {utilisateur && (
            <>
              {!dansUneRoom && (
                <>
                  <Link to="/play" className="text-base sm:text-lg button-blue-d px-3 sm:px-5 py-1">Jouer</Link>
                  <Link to="/profile" className="text-base sm:text-lg button-blue-d px-3 sm:px-5 py-1">Profil</Link>
                  <Link to="/friends" className="text-base sm:text-lg button-blue-d px-3 sm:px-5 py-1">
                    Amis{notificationsAmis > 0 && location.pathname !== '/friends' ? ' (!)' : ''}
                  </Link>
                </>
              )}
              <div className="ml-auto flex min-w-0 items-center gap-3">
                {}
                <span className="hidden lg:block min-w-0 max-w-[40vw] truncate text-sm text-gray-300" title={pseudo}>
                  {pseudo}
                </span>
                <button
                  type="button"
                  className="text-base sm:text-lg shrink-0 button-red-d px-3 sm:px-5 py-1 font-normal disabled:opacity-50 disabled:cursor-not-allowed"
                  onClick={() => { void handleLogout(); }}
                  disabled={boutonsBloques}
                  >
                  Déconnexion
                </button>
				<img src="/images/logout.png" alt="" className="h-8 sm:h-9 lg:h-20 w-auto object-contain shrink-0"/>
              </div>
            </>
          )}
        </nav>
      </header>

      <main className="flex-1 flex">
        <Outlet context={{ registerRoomLeave }} />
      </main>

      <Footer />
    </div>
  );
}
