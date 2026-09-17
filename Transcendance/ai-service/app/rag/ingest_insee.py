import asyncio
import xml.etree.ElementTree as ET

import httpx

from app.db import connect, ensure_schema, to_pgvector
from app.mistral_client import embed

BASE = "https://api.insee.fr/series/BDM/V1/data/SERIES_BDM/"

SERIES = [
    ("011818543",
     "le taux de chômage en France (France entière) est de {v} % de la population "
     "active, au sens du BIT, en données trimestrielles corrigées des variations "
     "saisonnières"),
    ("011818548",
     "le taux de chômage des moins de 25 ans en France (France entière) est de "
     "{v} %, au sens du BIT, en données trimestrielles corrigées des variations "
     "saisonnières"),
    ("011818581",
     "le taux de chômage de longue durée en France (France entière) est de {v} % "
     "de la population active, au sens du BIT, en données trimestrielles corrigées "
     "des variations saisonnières"),
    ("010757166",
     "le nombre de personnes mises en cause dans des affaires pénales "
     "poursuivables en France est de {v} sur l'année"),
    ("010608465",
     "le nombre d'agents de la fonction publique en France (hors Mayotte, "
     "trois versants confondus) est de {v}"),
    ("001687211",
     "le parc de logements en France métropolitaine compte {v} logements au total"),
    ("001687214",
     "le nombre de logements vacants en France métropolitaine est de {v}"),
]



def _source_url(idbank: str) -> str:
    return f"https://www.insee.fr/fr/statistiques/serie/{idbank}"


def _serie(client: httpx.Client, idbank: str):
    resp = client.get(BASE + idbank, timeout=30)
    resp.raise_for_status()
    root = ET.fromstring(resp.text)

    for el in root.iter():
        if el.tag.split("}")[-1] != "Series":
            continue
        titre = el.attrib.get("TITLE_FR", "")
        obs = [
            (c.attrib.get("TIME_PERIOD"), c.attrib.get("OBS_VALUE"))
            for c in el
            if c.tag.split("}")[-1] == "Obs" and c.attrib.get("OBS_VALUE")
        ]
        if not obs:
            return titre, None, None
        periode, valeur = max(obs, key=lambda o: o[0])

        facteur = 10 ** int(el.attrib.get("UNIT_MULT") or 0)
        if el.attrib.get("UNIT_MEASURE") == "E1000":
            facteur *= 1000
        if facteur != 1:
            valeur = str(round(float(valeur) * facteur))
        return titre, periode, valeur
    return "", None, None


def _lisible(periode: str) -> str:
    if "-Q" in periode:
        annee, trimestre = periode.split("-Q")
        rang = "1er" if trimestre == "1" else f"{trimestre}e"
        return f"au {rang} trimestre {annee}"
    return f"en {periode}"


async def ingest() -> None:
    ensure_schema()
    lignes = []
    with httpx.Client() as client:
        for idbank, modele in SERIES:
            try:
                titre, periode, valeur = _serie(client, idbank)
            except Exception as e:
                print(f"  (échec sur {idbank} : {type(e).__name__})")
                continue
            if valeur is None:
                print(f"  (aucune valeur pour {idbank})")
                continue
            affiche = valeur
            if "." not in valeur and valeur.lstrip("-").isdigit() and len(valeur.lstrip("-")) > 4:
                affiche = f"{int(valeur):,}".replace(",", " ")
            phrase = (
                f"Selon l'INSEE, {_lisible(periode)}, {modele.format(v=affiche)}. "
                f"Source : INSEE, Banque de données macroéconomiques (série {idbank})."
            )
            lignes.append((phrase, _source_url(idbank)))
            print(f"  {idbank} | {periode} -> {affiche} | {titre[:58]}")

    if not lignes:
        print("Aucun indicateur récupéré (API INSEE injoignable ?).")
        return

    with connect() as conn, conn.cursor() as cur:
        cur.execute("DELETE FROM documents WHERE source = 'insee';")
        conn.commit()

    vecteurs = await embed([phrase for phrase, _ in lignes])
    with connect() as conn, conn.cursor() as cur:
        for (phrase, url), vec in zip(lignes, vecteurs):
            cur.execute(
                "INSERT INTO documents (source, title, url, content, chunk_index, embedding) "
                "VALUES (%s, %s, %s, %s, %s, %s::vector)",
                ("insee", "Indicateur officiel (INSEE)", url, phrase, 0, to_pgvector(vec)),
            )
        conn.commit()

    print(f"{len(lignes)} indicateurs INSEE ingérés (source='insee').")


if __name__ == "__main__":
    asyncio.run(ingest())
