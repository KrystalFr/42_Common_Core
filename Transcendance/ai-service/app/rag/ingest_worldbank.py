import asyncio

import httpx

from app.db import connect, ensure_schema, to_pgvector
from app.mistral_client import embed

BASE = "https://api.worldbank.org/v2/country/{pays}/indicator/{code}"

PAYS = [
    ("FRA", "la France"),
    ("DEU", "l'Allemagne"),
    ("USA", "les États-Unis"),
    ("CHN", "la Chine"),
    ("WLD", "le monde"),
]

INDICATEURS = [
    ("NY.GDP.PCAP.CD",
     "le produit intérieur brut par habitant {pays_gen} est de {v} dollars américains", 0),
    ("SP.DYN.LE00.IN",
     "l'espérance de vie à la naissance {pays_gen} est de {v} ans", 1),
    ("EN.GHG.CO2.PC.CE.AR5",
     "les émissions de CO2 par habitant {pays_gen} sont de {v} tonnes par an", 2),
    ("SP.POP.TOTL",
     "la population totale {pays_gen} est de {v} habitants", 0),
]


def _source_url(code: str) -> str:
    return f"https://donnees.banquemondiale.org/indicateur/{code}"


def _valeur(client: httpx.Client, pays: str, code: str):
    """Renvoie (année, valeur brute) la plus récente, ou (None, None)."""
    resp = client.get(
        BASE.format(pays=pays, code=code),
        params={"format": "json", "per_page": 1, "mrnev": 1},
        timeout=45,
    )
    resp.raise_for_status()
    data = resp.json()
    if not isinstance(data, list) or len(data) < 2 or not data[1]:
        return None, None
    obs = data[1][0]
    if obs.get("value") is None:
        return None, None
    return obs.get("date"), obs["value"]


def _formate(valeur: float, decimales: int) -> str:
    if decimales == 0:
        return f"{round(valeur):,}".replace(",", " ")
    return f"{valeur:.{decimales}f}".replace(".", ",")


async def ingest() -> None:
    ensure_schema()
    lignes = []
    with httpx.Client() as client:
        for code, modele, decimales in INDICATEURS:
            for pays, libelle in PAYS:
                try:
                    annee, brut = _valeur(client, pays, code)
                except Exception as e:
                    print(f"  ({code}/{pays} : {type(e).__name__})")
                    continue
                if brut is None:
                    print(f"  (aucune valeur pour {code}/{pays})")
                    continue
                phrase = (
                    f"En {annee}, {modele.format(v=_formate(brut, decimales), pays_gen=libelle)}. "
                    f"Source : Banque mondiale (World Development Indicators)."
                )
                lignes.append((phrase, _source_url(code)))
                print(f"  {code:22} {pays:4}: {annee} -> {_formate(brut, decimales)}")

    if not lignes:
        print("Aucun indicateur récupéré (API Banque mondiale injoignable ?).")
        return

    with connect() as conn, conn.cursor() as cur:
        cur.execute("DELETE FROM documents WHERE source = 'banque_mondiale';")
        conn.commit()

    vecteurs = await embed([phrase for phrase, _ in lignes])
    with connect() as conn, conn.cursor() as cur:
        for (phrase, url), vec in zip(lignes, vecteurs):
            cur.execute(
                "INSERT INTO documents (source, title, url, content, chunk_index, embedding) "
                "VALUES (%s, %s, %s, %s, %s, %s::vector)",
                ("banque_mondiale", "Indicateur international (Banque mondiale)",
                 url, phrase, 0, to_pgvector(vec)),
            )
        conn.commit()

    print(f"{len(lignes)} indicateurs internationaux ingérés (source='banque_mondiale').")


if __name__ == "__main__":
    asyncio.run(ingest())
