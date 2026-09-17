import React from 'react';

export type PeriodValue = '1m' | '3m' | '6m' | '1y' | 'all' | 'custom';

export default function PeriodSelector({ value, onChange, from, to, onFromChange, onToChange }: {
  value: PeriodValue; onChange: (value: PeriodValue) => void;
  from: string; to: string; onFromChange: (v: string) => void; onToChange: (v: string) => void;
}) {
  return (
    <div className="flex flex-wrap items-end gap-2" aria-label="Période des statistiques">
      <div>
        <label htmlFor="stats-period" className="block text-xs text-white/40 mb-1">Période</label>
        <select id="stats-period" value={value} onChange={(e) => onChange(e.target.value as PeriodValue)}
          className="bg-noir-krystal border border-white/10 rounded-lg px-3 py-2 text-sm">
          <option value="1m">1 mois</option><option value="3m">3 mois</option><option value="6m">6 mois</option>
          <option value="1y">1 an</option><option value="all">Tout</option><option value="custom">Personnalisé</option>
        </select>
      </div>
      {value === 'custom' && <>
        <div><label htmlFor="stats-from" className="block text-xs text-white/40 mb-1">Début</label>
          <input id="stats-from" type="date" value={from} onChange={(e) => onFromChange(e.target.value)} className="bg-noir-krystal border border-white/10 rounded-lg px-3 py-2 text-sm" /></div>
        <div><label htmlFor="stats-to" className="block text-xs text-white/40 mb-1">Fin</label>
          <input id="stats-to" type="date" value={to} onChange={(e) => onToChange(e.target.value)} className="bg-noir-krystal border border-white/10 rounded-lg px-3 py-2 text-sm" /></div>
      </>}
    </div>
  );
}
