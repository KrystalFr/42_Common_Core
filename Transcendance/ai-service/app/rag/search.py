from app.db import connect, to_pgvector
from app.mistral_client import embed


async def search_similar(query: str, k: int = 5) -> list[dict]:
    [query_vector] = await embed([query])

    with connect() as conn, conn.cursor() as cur:
        cur.execute(
            """
            SELECT title, url, content,
                   embedding <=> %s::vector AS distance
            FROM documents
            ORDER BY distance ASC
            LIMIT %s
            """,
            (to_pgvector(query_vector), k),
        )
        rows = cur.fetchall()

    return [
        {"title": r[0], "url": r[1], "content": r[2], "distance": r[3]}
        for r in rows
    ]
