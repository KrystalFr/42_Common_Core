import re
import time
from urllib.parse import quote, urlparse

import httpx

from app.db import connect

ENTETES = {
    "User-Agent": "Mozilla/5.0 (X11; Linux x86_64) AppleWebKit/537.36 "
                  "(KHTML, like Gecko) Chrome/126.0.0.0 Safari/537.36",
    "Accept-Language": "fr-FR,fr;q=0.9",
}

DOMAINES = ["tf1info.fr", "20minutes.fr", "franceinfo.fr", "defacto-observatoire.fr", "rtbf.be"]

POST_FACEBOOK = re.compile(r'https?://(?:www\.|web\.|m\.)?facebook\.com/[^"\'<>\s\\]+?/videos/[0-9A-Za-z._-]+')
EMBED_YOUTUBE = re.compile(r'https?://(?:www\.)?youtube(?:-nocookie)?\.com/embed/([A-Za-z0-9_-]{11})')
WATCH_YOUTUBE = re.compile(r'https?://(?:www\.)?youtube\.com/watch\?v=([A-Za-z0-9_-]{11})')

COMPTES_MEDIAS = re.compile(r'facebook\.com/(?:TF1|LCI|20minutes|franceinfo|rtbf|sharer|dialog)', re.I)


def _embed_facebook(url_post: str) -> str:
    return ("https://www.facebook.com/plugins/video.php?href=" + quote(url_post, safe="")
            + "&show_text=false&width=560")


def _extraire(html: str) -> str | None:
    for brut in POST_FACEBOOK.findall(html):
        url = brut.replace("\\/", "/").rstrip('\\"\'')
        if COMPTES_MEDIAS.search(url):
            continue
        return _embed_facebook(url)
    for motif in (EMBED_YOUTUBE, WATCH_YOUTUBE):
        trouve = motif.search(html)
        if trouve:
            return f"https://www.youtube-nocookie.com/embed/{trouve.group(1)}"
    return None


def main(limite: int = 400) -> None:
    condition = " OR ".join(['"sourceUrl" LIKE %s'] * len(DOMAINES))
    with connect() as conn, conn.cursor() as cur:
        cur.execute(
            f'SELECT id, "sourceUrl", "truthLabel", text FROM "Claim" '
            f'WHERE "mediaRef" IS NULL AND ({condition}) LIMIT {limite}',
            tuple(f"%{d}%" for d in DOMAINES),
        )
        candidats = cur.fetchall()

    print(f"{len(candidats)} article(s) à examiner.\n")
    trouves, echecs = [], 0
    for claim_id, url, label, texte in candidats:
        try:
            r = httpx.get(url, headers=ENTETES, timeout=30, follow_redirects=True)
            html = r.text if r.status_code == 200 else ""
        except Exception:
            html = ""
        if not html:
            echecs += 1
        else:
            media = _extraire(html)
            if media:
                trouves.append((media, claim_id))
                plateforme = "facebook" if "facebook" in media else "youtube"
                print(f"  [{label:5s}|{plateforme:8s}] {texte[:62]}")
        time.sleep(1.5)

    if trouves:
        with connect() as conn, conn.cursor() as cur:
            cur.executemany('UPDATE "Claim" SET "mediaRef" = %s WHERE id = %s', trouves)
            conn.commit()

    print(f"\n{len(trouves)} vidéo(s) rattachée(s), {echecs} article(s) inaccessible(s).")


if __name__ == "__main__":
    main()
