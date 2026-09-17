import { Link } from 'react-router';


export default function Footer() {
  return (
    <footer className="p-4 text-white text-sm flex flex-wrap gap-x-4 gap-y-1">
      <span>© FactArena</span>
      <Link to="/privacy">Politique de confidentialité</Link>
      <Link to="/terms">CGU</Link>
    </footer>
  );
}
