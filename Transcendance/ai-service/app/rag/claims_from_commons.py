import hashlib
import secrets
import string
import time

import httpx

from app.db import connect

API = "https://commons.wikimedia.org/w/api.php"
ENTETES = {"User-Agent": "FactArena-42-project/0.1 (ft_transcendence; educational)"}

THEMES = [
    ("Cette vidéo montre une éruption volcanique.",
     "Category:Videos of volcanic eruptions"),
    ("Cette vidéo montre une tornade.",
     "Category:Videos of tornadoes"),
    ("Cette vidéo montre un impact de foudre.",
     "Category:Videos of lightning"),
    ("Cette vidéo montre une éclipse solaire.",
     "Category:Videos of solar eclipses"),
    ("Cette vidéo montre un lancement de fusée.",
     "Category:Videos of rocket launches"),
    ("Cette vidéo montre une avalanche.",
     "Category:Videos of avalanches"),
    ("Cette vidéo montre un geyser en éruption.",
     "Category:Videos of geysers"),
    ("Cette vidéo montre une baleine filmée en mer.",
     "Category:Videos of whales"),
    ("Cette vidéo montre une trombe marine.",
     "Category:Videos of waterspouts"),
    ("Cette vidéo montre les secousses d'un séisme.",
     "Category:Videos of earthquakes"),
    ("Cette vidéo montre une aurore polaire.",
     "Category:Videos of auroras"),
    ("Cette vidéo montre une éruption solaire vue depuis l'espace.",
     "Category:Videos of solar flares"),
]

EXTENSIONS = (".webm", ".ogv", ".mp4")
PAR_CATEGORIE = 5


def _lisible(url: str) -> bool:
    """L'URL pointe-t-elle vers un fichier vidéo jouable ?

    ⚠️ Commons ajoute parfois une chaîne de requête (« ?download&content=... »).
    Tester l'extension sur l'URL brute rejetait donc TOUS les fichiers : il faut
    couper avant le « ? ».
    """
    return url.split("?", 1)[0].lower().endswith(EXTENSIONS)


def _fichiers(client: httpx.Client, categorie: str):
    """Vidéos d'une catégorie Commons, avec gestion du quota (429)."""
    for tentative in range(3):
        r = client.get(
            API,
            params={
                "action": "query", "format": "json", "generator": "categorymembers",
                "gcmtitle": categorie, "gcmtype": "file", "gcmlimit": PAR_CATEGORIE * 3,
                "prop": "imageinfo", "iiprop": "url",
            },
            headers=ENTETES, timeout=30,
        )
        if r.status_code == 429:
            time.sleep(5 * (tentative + 1))
            continue
        r.raise_for_status()
        break
    else:
        return []

    pages = r.json().get("query", {}).get("pages", {})
    resultats = []
    for page in pages.values():
        info = (page.get("imageinfo") or [{}])[0]
        url, fiche = info.get("url") or "", info.get("descriptionurl") or ""
        if not fiche or not _lisible(url):
            continue
        resultats.append((page.get("title", ""), url, fiche))
        if len(resultats) >= PAR_CATEGORIE:
            break
    return resultats


def main() -> None:
    with connect() as conn, conn.cursor() as cur:
        cur.execute('SELECT "sourceUrl" FROM "Claim" WHERE "sourceUrl" IS NOT NULL;')
        deja = {r[0].rstrip("/") for r in cur.fetchall()}

    nouvelles = []
    with httpx.Client() as client:
        for affirmation, categorie in THEMES:
            try:
                trouves = _fichiers(client, categorie)
            except Exception as e:
                print(f"  ({categorie[9:40]} : {type(e).__name__})")
                continue
            if not trouves:
                print(f"  rien   {categorie[9:46]}")
                continue
            retenus = 0
            for _titre, url, fiche in trouves:
                if fiche.rstrip("/") in deja:
                    continue
                deja.add(fiche.rstrip("/"))
                cle = "commons:" + hashlib.sha1(fiche.encode()).hexdigest()[:16]
                nouvelles.append((cle, affirmation, "TRUE", "insolite_vrai", fiche, url))
                retenus += 1
            print(f"  {retenus:2} vidéo(s) {categorie[9:46]}")
            time.sleep(2)

    if not nouvelles:
        print("Aucune vidéo exploitable trouvée.")
        return

    alphabet = string.ascii_lowercase + string.digits
    avec_id = [("c" + "".join(secrets.choice(alphabet) for _ in range(24)), *n) for n in nouvelles]

    with connect() as conn, conn.cursor() as cur:
        cur.executemany(
            'INSERT INTO "Claim" (id, "sourceKey", text, "truthLabel", category, '
            '"sourceUrl", "mediaRef", "createdAt") '
            'VALUES (%s, %s, %s, %s::"ClaimTruthLabel", %s, %s, %s, now()) '
            'ON CONFLICT ("sourceKey") DO NOTHING',
            avec_id,
        )
        conn.commit()

    print(f"\n{len(nouvelles)} claims VRAIES avec vidéo créées (Wikimedia Commons).")


if __name__ == "__main__":
    main()
