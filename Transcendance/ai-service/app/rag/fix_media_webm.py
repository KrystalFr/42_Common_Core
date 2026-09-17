import time

import httpx

from app.db import connect

COMMONS_API = "https://commons.wikimedia.org/w/api.php"
UA = "FactArena-42-project/0.1 (educational; contact via 42 school)"

PREFERENCES = ["720p.vp9.webm", "480p.vp9.webm", "360p.vp9.webm", "240p.vp9.webm",
               "720p.webm", "480p.webm", "360p.webm", "240p.webm"]


def _titre_depuis_url(url: str) -> str | None:
    nu = url.split("?")[0]
    if "/transcoded/" in nu:
        return nu.split("/")[-2]
    if "upload.wikimedia.org" not in nu:
        return None
    return nu.split("/")[-1]


def _meilleur_webm(titre: str) -> str | None:
    for essai in range(4):
        try:
            r = httpx.get(COMMONS_API, params={
                "action": "query", "format": "json", "titles": f"File:{titre}",
                "prop": "videoinfo", "viprop": "derivatives",
            }, headers={"User-Agent": UA}, timeout=40)
            if r.status_code != 200:
                time.sleep(4)
                continue
            for page in r.json().get("query", {}).get("pages", {}).values():
                infos = (page.get("videoinfo") or [{}])[0]
                derives = {d.get("transcodekey"): d.get("src")
                           for d in (infos.get("derivatives") or []) if d.get("src")}
                for cle in PREFERENCES:
                    if derives.get(cle):
                        return derives[cle].split("?")[0]
            return None
        except Exception:
            time.sleep(4)
    return None


def main() -> None:
    with connect() as conn, conn.cursor() as cur:
        cur.execute(
            'UPDATE "Claim" SET "mediaRef" = split_part("mediaRef", \'?\', 1) '
            'WHERE "mediaRef" LIKE %s AND "mediaRef" LIKE %s',
            ("%upload.wikimedia.org%", "%?%"),
        )
        conn.commit()
        cur.execute(
            'SELECT id, "mediaRef" FROM "Claim" '
            'WHERE "mediaRef" LIKE %s AND (lower("mediaRef") LIKE %s OR lower("mediaRef") LIKE %s)',
            ("%upload.wikimedia.org%", "%.ogv%", "%.ogg%"),
        )
        a_reparer = cur.fetchall()

    print(f"{len(a_reparer)} vidéo(s) en Theora à remplacer.\n")
    corrections, echecs = [], []
    cache: dict[str, str | None] = {}

    for claim_id, media in a_reparer:
        titre = _titre_depuis_url(media)
        if not titre:
            echecs.append(media)
            continue
        if titre not in cache:
            cache[titre] = _meilleur_webm(titre)
            time.sleep(1.2)
        webm = cache[titre]
        if webm:
            corrections.append((webm, claim_id))
        else:
            echecs.append(titre)

    if corrections:
        with connect() as conn, conn.cursor() as cur:
            cur.executemany('UPDATE "Claim" SET "mediaRef" = %s WHERE id = %s', corrections)
            conn.commit()

    print(f"{len(corrections)} vidéo(s) basculée(s) en WebM.")
    if echecs:
        print(f"{len(echecs)} sans transcodage WebM (claims à écarter) :")
        for e in dict.fromkeys(echecs):
            print(f"  - {e[:90]}")


if __name__ == "__main__":
    main()
