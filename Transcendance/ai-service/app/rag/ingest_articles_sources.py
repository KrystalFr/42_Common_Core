import asyncio
from urllib.parse import urlparse

import httpx

from app.db import connect, ensure_schema, to_pgvector
from app.mistral_client import embed
from app.rag.chunking import chunk_text
from app.rag.ingest_factcheck import _ArticleText, _fetch

SOURCE = "factcheck_article"
BATCH = 50

HOTES_MEDIA = {
    "commons.wikimedia.org",
    "upload.wikimedia.org",
    "youtube.com",
    "youtu.be",
    "facebook.com",
    "twitter.com",
    "x.com",
}


def _via_archive(url: str) -> str:
    return f"https://web.archive.org/web/2024/{url}"


def _hote(url: str) -> str:
    try:
        return (urlparse(url).hostname or "").removeprefix("www.")
    except ValueError:
        return ""


def _urls_a_ingerer() -> list[str]:
    with connect() as conn, conn.cursor() as cur:
        cur.execute(
            'SELECT DISTINCT "sourceUrl" FROM public."Claim" '
            'WHERE "sourceUrl" IS NOT NULL AND "mediaRef" IS NOT NULL'
        )
        candidates = [row[0] for row in cur.fetchall()]
        cur.execute("SELECT DISTINCT url FROM documents WHERE source = %s", (SOURCE,))
        deja = {row[0] for row in cur.fetchall()}

    return [u for u in candidates if _hote(u) not in HOTES_MEDIA and u not in deja]


async def _ecrire(rows: list[dict], conn) -> None:
    if not rows:
        return
    vecteurs = await embed([r["content"] for r in rows])
    with conn.cursor() as cur:
        for r, vec in zip(rows, vecteurs):
            cur.execute(
                "INSERT INTO documents (source, title, url, content, chunk_index, embedding) "
                "VALUES (%s, %s, %s, %s, %s, %s::vector)",
                (SOURCE, r["title"], r["url"], r["content"], r["chunk_index"], to_pgvector(vec)),
            )
    conn.commit()


async def ingest() -> None:
    ensure_schema()
    urls = _urls_a_ingerer()
    if not urls:
        print("Rien à ingérer : tous les articles sources sont déjà en base.")
        return
    print(f"{len(urls)} article(s) à récupérer...")

    tampon: list[dict] = []
    reussis = echecs = 0
    with connect() as conn, httpx.Client() as client:
        for i, url in enumerate(urls, 1):
            html = _fetch(client, url)
            if not html:
                html = _fetch(client, _via_archive(url))
            if not html:
                echecs += 1
                print(f"  {i}/{len(urls)} échec réseau : {_hote(url)}")
                continue
            parseur = _ArticleText()
            parseur.feed(html)
            texte = parseur.text()
            if len(texte) < 400:
                echecs += 1
                print(f"  {i}/{len(urls)} trop peu de texte ({len(texte)} car.) : {_hote(url)}")
                continue
            titre = (parseur.title.strip() or url)[:200]
            morceaux = chunk_text(texte)
            for idx, morceau in enumerate(morceaux):
                tampon.append({"title": titre, "url": url, "content": morceau, "chunk_index": idx})
            reussis += 1
            print(f"  {i}/{len(urls)} {len(morceaux):3d} passages — {titre[:60]}")
            while len(tampon) >= BATCH:
                await _ecrire(tampon[:BATCH], conn)
                tampon = tampon[BATCH:]
        await _ecrire(tampon, conn)

    print(f"\n{reussis} article(s) ingéré(s), {echecs} inaccessible(s).")


if __name__ == "__main__":
    asyncio.run(ingest())
