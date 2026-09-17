import httpx

from app.db import connect

FORMATS_MORTS = (".ogv", ".ogg")


def _youtube_ok(client: httpx.Client, url: str) -> bool:
    identifiant = url.split("/embed/", 1)[-1].split("?", 1)[0]
    try:
        r = client.get(
            "https://www.youtube.com/oembed",
            params={"url": f"https://www.youtube.com/watch?v={identifiant}", "format": "json"},
            timeout=20,
        )
        return r.status_code == 200
    except Exception:
        return True


def main() -> None:
    with connect() as conn, conn.cursor() as cur:
        cur.execute('SELECT id, "mediaRef" FROM "Claim" WHERE "mediaRef" IS NOT NULL')
        claims = cur.fetchall()

    print(f"{len(claims)} claims avec vidéo à vérifier...")
    a_effacer: list[tuple[str]] = []
    raisons = {"format": 0, "youtube": 0}

    with httpx.Client() as client:
        for claim_id, media in claims:
            base = media.split("?", 1)[0].lower()
            if base.endswith(FORMATS_MORTS):
                a_effacer.append((claim_id,))
                raisons["format"] += 1
                continue
            if "/embed/" in media and "youtube" in media:
                if not _youtube_ok(client, media):
                    a_effacer.append((claim_id,))
                    raisons["youtube"] += 1

    if not a_effacer:
        print("Toutes les vidéos sont jouables.")
        return

    with connect() as conn, conn.cursor() as cur:
        cur.executemany('UPDATE "Claim" SET "mediaRef" = NULL WHERE id = %s', a_effacer)
        conn.commit()

    print(
        f"{len(a_effacer)} vidéos retirées "
        f"({raisons['format']} au format non lu par Chrome, "
        f"{raisons['youtube']} privées ou non intégrables)."
    )


if __name__ == "__main__":
    main()
