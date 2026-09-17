import os
import secrets
import string
import time

import httpx

from app.db import connect
from app.rag.export_claims import API, make_source_key, map_verdict
from app.rag.videos_depuis_factchecks import ENTETES, _extraire

REQUETES = [
    "délinquance", "immigration", "insécurité France", "criminalité étrangers",
    "statistiques délinquance", "demandeurs d'asile France", "OQTF",
    "immigration chiffres", "violences France chiffres",
    "titres de séjour France", "expulsions étrangers France", "prisons étrangers",
    "aide médicale d'État", "regroupement familial", "mineurs non accompagnés",
    "homicides France chiffres", "vols France statistiques", "asile Europe chiffres",
    "population immigrée France", "naturalisations France", "sécurité chiffres INSEE",
]


def main() -> None:
    cle = os.getenv("GOOGLE_API_KEY")
    if not cle:
        print("GOOGLE_API_KEY manquante dans .env.")
        return

    candidats: dict[str, dict] = {}
    for requete in REQUETES:
        for page in range(3):
            try:
                r = httpx.get(API, params={"key": cle, "query": requete,
                                           "languageCode": "fr", "pageSize": 50,
                                           "offset": page * 50}, timeout=40)
            except Exception:
                break
            if r.status_code != 200:
                break
            claims = r.json().get("claims", [])
            if not claims:
                break
            for c in claims:
                texte = (c.get("text") or "").strip()
                if not (30 <= len(texte) <= 260):
                    continue
                for rev in c.get("claimReview", []):
                    label = map_verdict(rev.get("textualRating", ""))
                    url = rev.get("url")
                    if not label or not url:
                        continue
                    candidats.setdefault(texte, {
                        "texte": texte, "label": label, "url": url,
                        "editeur": rev.get("publisher", {}).get("name", "factcheck"),
                    })
            time.sleep(1)

    vrais_dispo = sum(1 for d in candidats.values() if d["label"] == "TRUE")
    print(f"{len(candidats)} vérification(s) retenue(s) dont {vrais_dispo} VRAI.\n")
    if not candidats:
        return

    with connect() as conn, conn.cursor() as cur:
        cur.execute('SELECT "sourceKey" FROM "Claim" WHERE "sourceKey" IS NOT NULL')
        deja = {r[0] for r in cur.fetchall()}

    ordonnes = sorted(candidats.values(), key=lambda d: d["label"] != "TRUE")

    alphabet = string.ascii_lowercase + string.digits
    lignes, sans_video = [], 0
    for donnees in ordonnes:
        cle_source = make_source_key(donnees["editeur"], donnees["url"])
        if cle_source in deja:
            continue
        media = None
        try:
            page = httpx.get(donnees["url"], headers=ENTETES, timeout=30,
                             follow_redirects=True)
            if page.status_code == 200:
                media = _extraire(page.text)
        except Exception:
            media = None
        time.sleep(1.5)
        if not media:
            sans_video += 1
            continue
        lignes.append(("c" + "".join(secrets.choice(alphabet) for _ in range(24)),
                       cle_source, donnees["texte"], donnees["label"],
                       "societe_fr", donnees["url"], media))

    if lignes:
        with connect() as conn, conn.cursor() as cur:
            cur.executemany(
                'INSERT INTO "Claim" (id, "sourceKey", text, "truthLabel", category, '
                '"sourceUrl", "mediaRef", "createdAt") '
                'VALUES (%s, %s, %s, %s::"ClaimTruthLabel", %s, %s, %s, now()) '
                'ON CONFLICT ("sourceKey") DO NOTHING',
                lignes,
            )
            conn.commit()

    vrais = sum(1 for l in lignes if l[3] == "TRUE")
    print(f"{len(lignes)} clip(s) insérés ({vrais} VRAI / {len(lignes) - vrais} FAUX), "
          f"{sans_video} sans vidéo exploitable.\n")
    for ligne in lignes:
        print(f"  [{ligne[3]:5s}] {ligne[2][:78]}")


if __name__ == "__main__":
    main()
