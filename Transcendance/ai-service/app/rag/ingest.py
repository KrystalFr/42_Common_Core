import asyncio
import os

from datasets import load_dataset

from app.db import connect, ensure_schema, to_pgvector, truncate_documents
from app.mistral_client import embed
from app.rag.chunking import chunk_text

WIKI_DATASET = os.getenv("RAG_WIKI_DATASET", "wikimedia/wikipedia")
WIKI_CONFIG = os.getenv("RAG_WIKI_CONFIG", "20231101.fr")

MAX_ARTICLES = int(os.getenv("RAG_MAX_ARTICLES", "2000"))


BATCH = 200


def _wiki_url(title: str) -> str:
    return "https://fr.wikipedia.org/wiki/" + title.replace(" ", "_")


def _existing_titles() -> set[str]:
    with connect() as conn, conn.cursor() as cur:
        cur.execute("SELECT DISTINCT title FROM documents WHERE source = 'wikipedia_fr';")
        return {row[0] for row in cur.fetchall()}


def _documents(skip: set[str]) -> list[dict]:
    ds = load_dataset(WIKI_DATASET, WIKI_CONFIG, split="train", streaming=True)
    docs: list[dict] = []
    for i, article in enumerate(ds):
        if i >= MAX_ARTICLES:
            break
        title = article["title"]
        if title in skip:  # déjà ingéré lors d'un run précédent -> on saute
            continue
        url = _wiki_url(title)
        for idx, chunk in enumerate(chunk_text(article["text"])):
            docs.append(
                {"title": title, "url": url, "content": chunk, "chunk_index": idx}
            )
    return docs


async def ingest() -> None:
    ensure_schema()

    if os.getenv("RAG_RESET", "0") == "1":
        truncate_documents()
        print("Table vidée (RAG_RESET=1) : ré-ingestion complète.")

    done = _existing_titles()
    if done:
        print(f"{len(done)} articles déjà en base -> sautés (reprise).")

    docs = _documents(done)
    if not docs:
        print("Rien de nouveau à ingérer (corpus déjà complet, ou snapshot invalide).")
        return
    print(f"{len(docs)} passages à ingérer (depuis {MAX_ARTICLES} articles max)...")

    with connect() as conn:
        for start in range(0, len(docs), BATCH):
            batch = docs[start : start + BATCH]
            vectors = await embed([d["content"] for d in batch])
            with conn.cursor() as cur:
                for d, vec in zip(batch, vectors):
                    cur.execute(
                        "INSERT INTO documents "
                        "(source, title, url, content, chunk_index, embedding) "
                        "VALUES (%s, %s, %s, %s, %s, %s::vector)",
                        (
                            "wikipedia_fr",
                            d["title"],
                            d["url"],
                            d["content"],
                            d["chunk_index"],
                            to_pgvector(vec),
                        ),
                    )
            conn.commit()  # commit par paquet : la progression est sauvegardée
            print(f"  {min(start + BATCH, len(docs))}/{len(docs)}")

    print("Ingestion terminée.")


if __name__ == "__main__":
    asyncio.run(ingest())
