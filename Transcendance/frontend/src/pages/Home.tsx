import { Link } from 'react-router';

export default function Home() {
  return (
	<section className="grid w-full grid-cols-1 lg:grid-cols-2 gap-8 p-4 sm:p-8 bg-noir-krystal min-h-full">

		<div className="order-1 min-w-0 lg:row-start-1">
			<h1 className="text-5xl sm:text-6xl md:text-7xl lg:text-7xl xl:text-8xl 2xl:text-9xl font-bold text-white mt-2 whitespace-nowrap">FactArena</h1>
			<div className="mt-6 mb-8 flex items-center gap-4">
				<Link to="/play" className="button-blue-d px-10 sm:px-15 py-3 text-2xl sm:text-[30px]">Jouer</Link>
				<img src="/images/allcolours.gif" alt="" className="h-20 lg:h-36 w-auto object-contain shrink-0"/>
			</div>
		</div>

		<div className="order-2 min-w-0 lg:row-start-1 p-4 sm:p-8 grid gap-4 text-white bg-gris-krystal rounded-xl mb-2">
			<div className="min-w-0 lg:row-start-1 bg-noir-krystal rounded-xl p-5 flex items-center justify-start">
				<h2 className="text-xl sm:text-2xl md:text-3xl font-semibold leading-tight text-left">REGARDE UNE VIDEO</h2>
			</div>
			<div className="min-w-0 lg:row-start-2 bg-noir-krystal rounded-xl p-5 flex items-center justify-start">
				<h2 className="text-xl sm:text-2xl md:text-3xl font-semibold leading-tight text-right">MISE SUR <span className="text-2xl sm:text-3xl md:text-5xl text-emerald-300">FACT</span> OU <span className="text-2xl sm:text-3xl md:text-5xl text-red-300">FAKE</span></h2>
			</div>
			<div className="min-w-0 lg:row-start-3 bg-noir-krystal rounded-xl p-5 flex items-center justify-start">
				<h2 className="text-xl sm:text-2xl md:text-3xl font-semibold leading-tight text-left">DISCUTE AVEC LES AUTRES JOUEURS EN DIRECT</h2>
			</div>
		</div>

    </section>
  );
}
