
import { Navigate, Outlet, useLocation, useOutletContext } from 'react-router';
import { useAuth } from '../auth/AuthContext';

export default function ProtectedRoute() {
  const { utilisateur, chargement } = useAuth();
  const layoutContext = useOutletContext();
  const emplacement = useLocation();


  if (chargement) return null;


  if (!utilisateur) {
    return <Navigate to="/login" replace state={{ depuis: emplacement.pathname }} />;
  }

  return <Outlet context={layoutContext} />;
}
