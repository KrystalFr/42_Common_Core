import gzip
import json
import pathlib

from app.db import connect

SORTIE = pathlib.Path("/app/export/rag-noyau.json.gz")

SOURCES = ("factcheck", "factcheck_article", "insee", "eurostat", "banque_mondiale")


def main() -> None:
    with connect() as conn, conn.cursor() as cur:
        cur.execute(
            """SELECT content, url, title, source, embedding::text
               FROM rag.documents WHERE source = ANY(%s) ORDER BY source, url""",
            (list(SOURCES),),
        )
        lignes = cur.fetchall()

    passages = [
        {"content": c, "url": u, "title": t, "source": s, "embedding": e}
        for c, u, t, s, e in lignes
    ]

    SORTIE.parent.mkdir(parents=True, exist_ok=True)
    with gzip.open(SORTIE, "wt", encoding="utf-8") as f:
        json.dump(passages, f, ensure_ascii=False)

    poids = SORTIE.stat().st_size / 1024 / 1024
    print(f"{len(passages)} passages exportés vers {SORTIE} ({poids:.1f} Mo)")
    par_source: dict[str, int] = {}
    for p in passages:
        par_source[p["source"]] = par_source.get(p["source"], 0) + 1
    for source, n in sorted(par_source.items(), key=lambda x: -x[1]):
        print(f"  {source:18s} {n}")


if __name__ == "__main__":
    main()
