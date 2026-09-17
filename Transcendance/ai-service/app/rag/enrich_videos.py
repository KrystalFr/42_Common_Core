import os
import re
from urllib.parse import quote

import httpx

from app.db import connect

YOUTUBE_RE = re.compile(
    r"https?://(?:www\.)?(?:youtube\.com/(?:watch\?v=|embed/|shorts/)|youtu\.be/)([\w-]{6,})",
    re.I,
)

FACEBOOK_RE = re.compile(
    r"https?://(?:www\.)?(?:facebook\.com/[\w.]+/videos/\d+|fb\.watch/[\w-]+)",
    re.I,
)

LIMITE = int(os.getenv("VIDEO_ENRICH_LIMIT", "40"))
ENTETES = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
    "(KHTML, like Gecko) Chrome/120 Safari/537.36",
}


def _html(client: httpx.Client, url: str) -> str:
    """HTML de l'article ; bascule sur l'archive si le site refuse le robot."""
    for cible in (url, "https://web.archive.org/web/2id_/" + url):
        try:
            r = client.get(cible, headers=ENTETES, timeout=25, follow_redirects=True)
            if r.status_code == 200 and len(r.text) > 500:
                return r.text
        except Exception:
            continue
    return ""


def _embed(html: str) -> str | None:
    ids = YOUTUBE_RE.findall(html)
    if ids:
        meilleur = max(set(ids), key=ids.count)
        return f"https://www.youtube-nocookie.com/embed/{meilleur}"

    liens = FACEBOOK_RE.findall(html)
    if liens:
        meilleur = max(set(liens), key=liens.count)
        return (
            "https://www.facebook.com/plugins/video.php?href="
            + quote(meilleur, safe="")
            + "&show_text=false"
        )
    return None


def _integrable(client: httpx.Client, video_id: str) -> bool:
    try:
        r = client.get(
            "https://www.youtube.com/oembed",
            params={"url": f"https://www.youtube.com/watch?v={video_id}", "format": "json"},
            timeout=20,
        )
        return r.status_code == 200
    except Exception:
        return False


def main() -> None:
    with connect() as conn, conn.cursor() as cur:
        seulement = os.getenv("VIDEO_ENRICH_ONLY", "").upper()
        filtre = ' AND "truthLabel" = %s' if seulement in ("TRUE", "FALSE") else ""
        params: tuple = (seulement, LIMITE) if filtre else (LIMITE,)
        cur.execute(
            'SELECT id, "sourceUrl" FROM "Claim" '
            'WHERE "mediaRef" IS NULL AND "sourceUrl" IS NOT NULL'
            + filtre
            + " LIMIT %s",
            params,
        )
        claims = cur.fetchall()

    if not claims:
        print("Aucune claim à enrichir (toutes ont déjà une vidéo).")
        return

    print(f"{len(claims)} claims à traiter...")
    trouves: list[tuple[str, str]] = []
    with httpx.Client() as client:
        for claim_id, url in claims:
            embed = _embed(_html(client, url))
            if not embed:
                print(f"  rien {url[:64]}")
                continue
            if "youtube-nocookie" in embed:
                video_id = embed.rsplit("/", 1)[-1]
                if not _integrable(client, video_id):
                    print(f"  NON  {video_id} refuse l'intégration -> ignorée")
                    continue
                print(f"  OK   youtube {video_id}  <- {url[:52]}")
            else:
                print(f"  OK   facebook          <- {url[:52]}")
            trouves.append((embed, claim_id))

    if not trouves:
        print("\nAucune vidéo YouTube embarquable trouvée.")
        return

    with connect() as conn, conn.cursor() as cur:
        cur.executemany('UPDATE "Claim" SET "mediaRef" = %s WHERE id = %s', trouves)
        conn.commit()

    print(f"\n{len(trouves)} claims enrichies sur {len(claims)} examinées.")


if __name__ == "__main__":
    main()
