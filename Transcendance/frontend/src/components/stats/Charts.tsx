import {
  Cell,
  Legend,
  Line,
  LineChart,
  Bar,
  BarChart,
  ComposedChart,
  Pie,
  PieChart,
  PolarAngleAxis,
  PolarGrid,
  PolarRadiusAxis,
  Radar,
  RadarChart,
  ReferenceLine,
  ResponsiveContainer,
  Tooltip,
  XAxis,
  YAxis,
} from 'recharts';

const pct = (v: number) => `${Math.round((Number.isFinite(v) ? v : 0) * 100)} %`;

const chartColors = ['#60a5fa', '#34d399', '#f59e0b', '#f87171', '#a78bfa', '#22d3ee', '#fb7185', '#c084fc'];

export function BalanceEvolution({ points }: { points: { round: number; date: string; balanceAfter: number }[] }) {
  if (!points.length) return <EmptyStats text="Aucune manche résolue sur cette période." />;
  const data = points.map((point) => ({
    ...point,
    label: `M${point.round}`,
  }));
  return (
    <div className="w-full h-72 mt-3">
      <ResponsiveContainer width="100%" height="100%">
        <LineChart data={data} margin={{ top: 10, right: 20, left: 4, bottom: 5 }}>
          <XAxis dataKey="label" tick={{ fill: 'currentColor', opacity: 0.5, fontSize: 11 }} />
          <YAxis tick={{ fill: 'currentColor', opacity: 0.5, fontSize: 11 }} width={52} />
          <Tooltip
            contentStyle={{ background: '#151515', border: '1px solid rgba(255,255,255,.08)', borderRadius: 12 }}
            labelStyle={{ color: '#fff' }}
            formatter={(value) => [`${value} cr`, 'Solde']}
            labelFormatter={(label, payload) => {
              const item = payload?.[0]?.payload as { date?: string } | undefined;
              return item?.date ? `${label} · ${new Date(item.date).toLocaleDateString('fr-FR')}` : label;
            }}
          />
          <Line type="monotone" dataKey="balanceAfter" name="Solde" stroke="#60a5fa" strokeWidth={3} dot={{ r: 4 }} activeDot={{ r: 6 }} />
        </LineChart>
      </ResponsiveContainer>
    </div>
  );
}

export type RoomChartPlayer = { id: string; name: string; color: string };

const pastelColors = ['#A7C7E7', '#B8E0D2', '#F8C8A8', '#D7C4F2', '#F3B8C6', '#C7D9A7', '#F4D58D', '#B9D7E8'];

type RoomPlayer = { user: { id: string; displayName: string | null; email: string }; initialBalance?: number };
type RoomBet = { userId: string; player: string; stake: number; answer: string | null; result: string | null; variation: number | null };
type RoomRound = { round: number; status: string; bets: RoomBet[]; clips: { player: string; correct: boolean }[] };

export function RoomCreditsChart({ players, roundDetails, evolution }: { players: RoomPlayer[]; roundDetails: RoomRound[]; evolution: Record<string, string | number | null>[] }) {
  if (!players.length) return <EmptyStats text="Les statistiques apparaîtront après les premières réponses." />;
  const orderedPlayers = [...players]
    .sort((a, b) => (a.user.displayName || a.user.email).localeCompare(b.user.displayName || b.user.email, 'fr'))
    .map((player, index) => ({ id: player.user.id, name: player.user.displayName || player.user.email, color: pastelColors[index % pastelColors.length] }));

  const rounds = [...roundDetails].filter(r => r.round > 0).sort((a,b) => a.round-b.round);
  const evolutionByRound = new Map(evolution.map(point => [Number(point.round), point]));
  const baseline = evolutionByRound.get(0) ?? {};
  const data: Record<string, unknown>[] = [{ x: 0, kind: 'balance', round: 0, label: 'M0', ...Object.fromEntries(orderedPlayers.map(p => [p.id, Number(baseline[p.id] ?? 0)])) }];

  rounds.forEach((round) => {
    const bets = new Map(round.bets.map(b => [b.userId, b]));
    const stakePoint: Record<string, unknown> = { x: round.round - 0.5, kind: 'stake', round: round.round };
    orderedPlayers.forEach(player => {
      const bet = bets.get(player.id);
      if (bet?.answer) stakePoint[`stake:${player.id}`] = bet.stake;
    });
    if (Object.keys(stakePoint).length > 3) data.push(stakePoint);

    const balancePoint = evolutionByRound.get(round.round);
    if (!balancePoint) return;
    data.push({ x: round.round, kind: 'balance', round: round.round, ...Object.fromEntries(orderedPlayers.map(p => [p.id, Number(balancePoint[p.id] ?? 0)])) });
  });

  const maxRound = Math.max(1, ...rounds.map(r => r.round));
  return <div className="w-full h-[24rem] mt-3"><ResponsiveContainer width="100%" height="100%"><ComposedChart data={data} margin={{ top: 12, right: 18, left: 4, bottom: 10 }}>
    <XAxis dataKey="x" type="number" domain={[0, maxRound]} ticks={Array.from({length:maxRound+1},(_,i)=>i)} tickFormatter={v=>`M${v}`} tick={{ fill:'currentColor', opacity:.5, fontSize:11 }} axisLine={{ stroke:'rgba(255,255,255,.12)' }} />
    <YAxis tick={{ fill:'currentColor', opacity:.5, fontSize:11 }} width={52} axisLine={false} />
    <Tooltip contentStyle={{ background:'#151515', border:'1px solid rgba(255,255,255,.10)', borderRadius:14, boxShadow:'0 14px 35px rgba(0,0,0,.25)' }} labelStyle={{color:'#fff',fontWeight:700,marginBottom:6}}
      labelFormatter={(_, payload) => { const item=payload?.[0]?.payload as {kind?:string;round?:number}|undefined; return item?.kind==='stake' ? `Mises · entre M${Math.max(0,(item.round??1)-1)} et M${item.round}` : `Manche ${item?.round??0}`; }}
      formatter={(value,name) => { const raw=String(name); const player=orderedPlayers.find(p=>p.id===raw || `stake:${p.id}`===raw); return player ? [`${Number(value??0)} cr`, raw.startsWith('stake:') ? `${player.name} · mise` : `${player.name} · crédits`] : [value as number,raw]; }} />
    <Legend wrapperStyle={{paddingTop:8,fontSize:11}} formatter={value => <span className="text-white/60">{orderedPlayers.find(p=>p.id===value)?.name ?? value}</span>} />
    {orderedPlayers.map(p=><Bar key={`stake-${p.id}`} dataKey={`stake:${p.id}`} name={`stake:${p.id}`} fill={p.color} fillOpacity={.48} barSize={10} legendType="none" radius={[4,4,0,0]} />)}
    {orderedPlayers.map(p=><Line key={p.id} type="monotone" dataKey={p.id} name={p.id} stroke={p.color} strokeWidth={2.5} dot={{r:3,fill:p.color}} activeDot={{r:5}} connectNulls />)}
  </ComposedChart></ResponsiveContainer></div>;
}

export function RoomAccuracyChart({ players, roundDetails }: { players: RoomPlayer[]; roundDetails: RoomRound[] }) {
  const ordered=[...players].sort((a,b)=>(a.user.displayName||a.user.email).localeCompare(b.user.displayName||b.user.email,'fr'));
  const resolved=roundDetails.filter(r=>['REVEALED','FINISHED'].includes(r.status) && r.clips.length>0);
  if (resolved.length < 2) return <EmptyStats text="La précision sera disponible à partir de la deuxième manche." />;
  const data=ordered.map(p=>{ const name=p.user.displayName||p.user.email; const clips=resolved.flatMap(r=>r.clips.filter(c=>c.player===name)); return {name,precision:clips.length?Math.round(clips.filter(c=>c.correct).length/clips.length*100):0}; });
  return <div className="w-full h-[10.5rem] mt-2"><ResponsiveContainer width="100%" height="100%"><ComposedChart data={data} margin={{top:5,right:8,left:0,bottom:0}}><XAxis dataKey="name" tick={{fill:'currentColor',opacity:.5,fontSize:10}}/><YAxis domain={[0,100]} tick={{fill:'currentColor',opacity:.4,fontSize:9}} width={38}/><Tooltip contentStyle={{background:'#151515',border:'1px solid rgba(255,255,255,.1)',borderRadius:12}} separator="" formatter={v=>[`${Number(v??0)} %`,'']}/><Bar dataKey="precision" name="Précision" radius={[5,5,0,0]}>{data.map((e,i)=><Cell key={e.name} fill={pastelColors[i%pastelColors.length]} fillOpacity={.78}/>)}</Bar></ComposedChart></ResponsiveContainer></div>;
}

export function RoomCumulativeGainChart({ players, roundDetails }: { players: RoomPlayer[]; roundDetails: RoomRound[] }) {
  const ordered=[...players].sort((a,b)=>(a.user.displayName||a.user.email).localeCompare(b.user.displayName||b.user.email,'fr'));
  const data=ordered.map(p=>{ const name=p.user.displayName||p.user.email; let gagne=0,perdu=0; roundDetails.flatMap(r=>r.bets.filter(b=>b.player===name)).forEach(b=>{ const v=b.variation??0; v>=0?gagne+=v:perdu+=v; });
  return {name,gagne,perdu,solde:gagne+perdu}; });
  if (!data.some(d=>d.gagne||d.perdu)) return <EmptyStats text="Les gains et pertes apparaîtront après les premières manches." />;
  return <div className="w-full h-[10.5rem] mt-2">
    <ResponsiveContainer width="100%" height="100%">
      <BarChart data={data} stackOffset="sign" margin={{top:5,right:8,left:0,bottom:0}}>
        <XAxis dataKey="name" tick={{fill:'currentColor',opacity:.5,fontSize:10}}/>
        <YAxis tick={{fill:'currentColor',opacity:.4,fontSize:9}} width={38}/>
        <Tooltip contentStyle={{background:'#151515',border:'1px solid rgba(255,255,255,.1)',borderRadius:12}} separator="" formatter={(v,n)=>[`${Number(v??0)} cr`,n==='gagne'?'Gagné ':n==='perdu'?'Perdu ':'Solde ']}/>
          <Legend wrapperStyle={{fontSize:10}}/><ReferenceLine y={0} stroke="currentColor" strokeOpacity={.25}/>
          <Bar dataKey="gagne" name="Gagné " stackId="gains-pertes" fill="#B8E0D2" fillOpacity={.8} radius={[5,5,0,0]}/>
          <Bar dataKey="perdu" name="Perdu " stackId="gains-pertes" fill="#F3B8C6" fillOpacity={.8} radius={[0,0,5,5]}/>
          <Bar dataKey="solde" name="Solde " fill="#F5D76E" fillOpacity={.95} radius={[5,5,5,5]}/>
          </BarChart></ResponsiveContainer></div>;
}

export function AccuracyRadar({ categories }: { categories: { category: string; accuracy: number; total: number }[] }) {
  if (!categories.length) return <EmptyStats text="Les statistiques apparaîtront après les premières réponses." />;
  const data = [...categories]
    .sort((a, b) => b.total - a.total || a.category.localeCompare(b.category, 'fr'))
    .slice(0, 8)
    .map((item) => ({ ...item, precision: Math.round(item.accuracy * 100) }));

  return (
    <div className="w-full h-80 mt-3">
      <ResponsiveContainer width="100%" height="100%">
        <RadarChart data={data} cx="50%" cy="50%" outerRadius="68%">
          <PolarGrid stroke="rgba(255,255,255,.12)" />
          <PolarAngleAxis dataKey="category" tick={{ fill: 'currentColor', opacity: 0.65, fontSize: 10 }} />
          <PolarRadiusAxis angle={90} domain={[0, 100]} tick={{ fill: 'currentColor', opacity: 0.35, fontSize: 9 }} />
          <Radar name="Précision" dataKey="precision" stroke="#60a5fa" fill="#60a5fa" fillOpacity={0.22} strokeWidth={2} />
          <Tooltip
            contentStyle={{ background: '#151515', border: '1px solid rgba(255,255,255,.08)', borderRadius: 12 }}
            formatter={(value) => [`${value} %`, 'Précision']}
          />
        </RadarChart>
      </ResponsiveContainer>
    </div>
  );
}

export function GainLossPie({ bestGain, worstLoss }: { bestGain: number; worstLoss: number }) {
  const data = [
    { name: 'Meilleur gain ', value: Math.max(0, bestGain) },
    { name: 'Meilleure perte ', value: Math.abs(worstLoss) },
  ].filter((item) => item.value > 0);
  if (!data.length) return <EmptyStats text="Les gains et pertes apparaîtront après les premières manches." />;

  return (
    <div className="w-full h-80 flex flex-col items-center justify-center">
      <div className="w-full h-64">
        <ResponsiveContainer width="100%" height="100%">
          <PieChart>
            <Pie data={data} dataKey="value" nameKey="name" innerRadius="55%" outerRadius="78%" paddingAngle={5} cornerRadius="12%" stroke="none">
              {data.map((entry, index) => <Cell key={entry.name} fill={entry.name === 'Meilleur gain ' ? '#34d399' : '#f87171'} />)}
            </Pie>
            <Tooltip
              contentStyle={{ background: '#151515', border: '1px solid rgba(255,255,255,.08)', borderRadius: 12 }}
              separator=""
              formatter={(value, name) => [`${value} cr`, ` ${name}`]}
            />
            <Legend verticalAlign="bottom" formatter={(value) => <span className="text-xs text-white/65">{value}</span>} />
          </PieChart>
        </ResponsiveContainer>
      </div>
      <div className="flex gap-6 text-sm">
        <span><strong className="text-emerald-400">+{bestGain}</strong> cr</span>
        <span><strong className="text-red-400">-{Math.abs(worstLoss)}</strong> cr</span>
      </div>
    </div>
  );
}

export function EmptyStats({ text }: { text: string }) { return <p className="py-8 text-center text-sm text-white/35">{text}</p>; }
export { pct };
