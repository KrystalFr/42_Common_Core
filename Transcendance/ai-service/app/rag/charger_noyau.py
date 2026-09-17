import gzip
import json
import logging
import pathlib

from app.db import connect, ensure_schema

FICHIER = pathlib.Path(__file__).resolve().parents[2] / "data" / "rag-noyau.json.gz"
logger = logging.getLogger("uvicorn.error")


def charger_si_vide() -> int:
    """Renvoie le nombre de passages insérés (0 si rien à faire)."""
    if not FICHIER.exists():
        logger.warning("Noyau vectoriel introuvable (%s), index laissé vide.", FICHIER)
        return 0

    ensure_schema()
    with connect() as conn, conn.cursor() as cur:
        cur.execute("SELECT count(*) FROM rag.documents")
        if cur.fetchone()[0] > 0:
            return 0

    with gzip.open(FICHIER, "rt", encoding="utf-8") as f:
        passages = json.load(f)

    with connect() as conn, conn.cursor() as cur:
        cur.executemany(
            """INSERT INTO rag.documents (content, url, title, source, embedding)
               VALUES (%s, %s, %s, %s, %s::vector)""",
            [(p["content"], p["url"], p["title"], p["source"], p["embedding"])
             for p in passages],
        )
        conn.commit()
    return len(passages)


if __name__ == "__main__":
    logging.basicConfig(level=logging.INFO)
    n = charger_si_vide()
    print(f"{n} passages chargés" if n else "Index déjà peuplé, rien à faire.")
