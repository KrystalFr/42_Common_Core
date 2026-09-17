import os
import secrets
import string
import time

import httpx

from app.db import connect
from app.rag.export_claims import API, make_source_key, map_verdict
from app.rag.videos_depuis_factchecks import ENTETES, _extraire

REQUETES = [
    "Trump video authentic", "Trump footage", "Trump video shows",
    "Trump AI video", "Trump video altered", "Trump deepfake video",
]

MOTS_VIDEO = ("video", "footage", "clip", "recording")
MOTS_IMAGE = ("photograph", "photo ", "a photo", "image", "screenshot", "picture")


def _est_une_video(texte: str) -> bool:
    bas = texte.lower()
    if not any(m in bas for m in MOTS_VIDEO):
        return False
    return not any(m in bas for m in MOTS_IMAGE)


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
                                           "languageCode": "en", "pageSize": 50,
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
                if "trump" not in texte.lower() or not _est_une_video(texte):
                    continue
                for rev in c.get("claimReview", []):
                    label = map_verdict(rev.get("textualRating", ""))
                    url = rev.get("url")
                    if not label or not url:
                        continue
                    editeur = rev.get("publisher", {}).get("name", "factcheck")
                    candidats.setdefault(texte, {
                        "texte": texte, "label": label, "url": url, "editeur": editeur,
                    })
            time.sleep(1)

    print(f"{len(candidats)} vérification(s) de vidéo retenue(s).\n")
    if not candidats:
        return

    with connect() as conn, conn.cursor() as cur:
        cur.execute('SELECT "sourceKey" FROM "Claim" WHERE "sourceKey" IS NOT NULL')
        deja = {r[0] for r in cur.fetchall()}

    alphabet = string.ascii_lowercase + string.digits
    lignes = []
    sans_video = 0
    for donnees in candidats.values():
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
                       "trump_media", donnees["url"], media))

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
          f"{sans_video} écarté(s) faute de vidéo dans l'article.\n")
    for ligne in lignes:
        plateforme = "facebook" if "facebook" in ligne[6] else "youtube"
        print(f"  [{ligne[3]:5s}|{plateforme:8s}] {ligne[2][:66]}")


if __name__ == "__main__":
    main()
