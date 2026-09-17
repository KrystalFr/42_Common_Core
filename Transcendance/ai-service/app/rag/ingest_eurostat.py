import asyncio

import httpx

from app.db import connect, ensure_schema, to_pgvector
from app.mistral_client import embed

BASE = "https://ec.europa.eu/eurostat/api/dissemination/statistics/1.0/data/"
HEADERS = {"User-Agent": "FactArena-42-project/0.1 (ft_transcendence; educational)"}

ZONES = [
    ("FR", "en France", "de la France"),
    ("EU27_2020", "dans l'Union européenne", "de l'Union européenne"),
]

INDICATEURS = [
    ("chômage",
     "le taux de chômage {loc} est de {v} % de la population active",
     "une_rt_m", {"sex": "T", "age": "TOTAL", "s_adj": "SA", "unit": "PC_ACT"}),
    ("chômage des jeunes",
     "le taux de chômage des jeunes de moins de 25 ans {loc} est de {v} %",
     "une_rt_m", {"sex": "T", "age": "Y_LT25", "s_adj": "SA", "unit": "PC_ACT"}),
    ("inflation",
     "l'inflation annuelle {loc} (indice des prix à la consommation harmonisé) est de {v} %",
     "prc_hicp_manr", {"coicop": "CP00", "unit": "RCH_A"}),
    ("dette publique",
     "la dette publique {gen} représente {v} % du PIB",
     "gov_10dd_edpt1", {"na_item": "GD", "sector": "S13", "unit": "PC_GDP"}),
    ("déficit public",
     "le déficit public {gen} représente {v} % du PIB",
     "gov_10dd_edpt1", {"na_item": "B9", "sector": "S13", "unit": "PC_GDP"}),
    ("croissance du PIB",
     "la croissance du PIB réel {gen} est de {v} % sur l'année",
     "tec00115", {"na_item": "B1GQ", "unit": "CLV_PCH_PRE"}),
    ("dépense publique",
     "la dépense publique {gen} représente {v} % du PIB",
     "gov_10a_main", {"na_item": "TE", "sector": "S13", "unit": "PC_GDP"}),
    ("immigration",
     "le nombre d'immigrants ayant établi leur résidence {loc} est de {v} "
     "sur l'année (toutes nationalités confondues, définition Eurostat)",
     "migr_imm1ctz", {"agedef": "COMPLET", "age": "TOTAL", "sex": "T", "citizen": "TOTAL", "unit": "NR"}),
]


def _source_url(dataset: str) -> str:
    return f"https://ec.europa.eu/eurostat/databrowser/view/{dataset}/default/table?lang=fr"


def _latest(client: httpx.Client, dataset: str, filtres: dict, geo: str = "FR"):
    params = {**filtres, "geo": geo, "format": "JSON", "lang": "FR"}
    data = client.get(BASE + dataset, params=params, headers=HEADERS, timeout=25).json()
    index = data["dimension"]["time"]["category"]["index"]
    valeurs = data["value"]
    for periode in sorted(index, key=lambda k: index[k], reverse=True):
        v = valeurs.get(str(index[periode]))
        if v is not None:
            return periode, v
    return None, None


async def ingest() -> None:
    ensure_schema()
    lignes = []
    with httpx.Client() as client:
        for libelle, modele, dataset, filtres in INDICATEURS:
            for geo, loc, gen in ZONES:
                periode, v = _latest(client, dataset, filtres, geo)
                if v is None:
                    print(f"  (aucune valeur pour {libelle} / {geo})")
                    continue
                phrase = (
                    f"En {periode}, {modele.format(v=v, loc=loc, gen=gen)}. "
                    f"Source : Eurostat (ESTAT)."
                )
                lignes.append((phrase, _source_url(dataset)))
                print(f"  {libelle:22} {geo:10}: {periode} -> {v}")

    if not lignes:
        print("Aucun indicateur récupéré (Eurostat injoignable ?).")
        return

    with connect() as conn, conn.cursor() as cur:
        cur.execute("DELETE FROM documents WHERE source = 'eurostat';")
        conn.commit()

    vecteurs = await embed([phrase for phrase, _ in lignes])
    with connect() as conn, conn.cursor() as cur:
        for (phrase, url), vec in zip(lignes, vecteurs):
            cur.execute(
                "INSERT INTO documents (source, title, url, content, chunk_index, embedding) "
                "VALUES (%s, %s, %s, %s, %s, %s::vector)",
                ("eurostat", "Indicateur officiel (Eurostat)", url, phrase, 0, to_pgvector(vec)),
            )
        conn.commit()
    print(f"{len(lignes)} indicateurs Eurostat ingérés (source='eurostat').")


if __name__ == "__main__":
    asyncio.run(ingest())
