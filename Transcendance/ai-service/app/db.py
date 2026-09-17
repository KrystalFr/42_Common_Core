import psycopg

from app.config import settings

AI_SCHEMA = "rag"

EMBED_DIM = 1024

def truncate_documents() -> None:
    with connect() as conn, conn.cursor() as cur:
        cur.execute("TRUNCATE documents RESTART IDENTITY;")
        conn.commit()

def connect() -> psycopg.Connection:
    return psycopg.connect(
        settings.database_url, options=f"-c search_path={AI_SCHEMA},public"
    )

def to_pgvector(values: list[float]) -> str:
    return "[" + ",".join(f"{x:.6f}" for x in values) + "]"

def ensure_schema() -> None:
    with connect() as conn, conn.cursor() as cur:
        cur.execute(f"CREATE SCHEMA IF NOT EXISTS {AI_SCHEMA};")
        cur.execute("CREATE EXTENSION IF NOT EXISTS vector SCHEMA public;")
        cur.execute(
            f"""
            CREATE TABLE IF NOT EXISTS documents (
                id          serial PRIMARY KEY,
                source      text,            -- provenance (ex. 'wikipedia_fr') : pour mélanger des corpus plus tard
                title       text,            -- titre du document (sert de citation dans les réponses)
                url         text,            -- lien vers la source (citation)
                content     text NOT NULL,   -- le passage réellement vectorisé (un chunk)
                chunk_index int,             -- position du chunk dans le document d'origine
                embedding   vector({EMBED_DIM})
            );
            """
        )
        cur.execute(
            """
            CREATE INDEX IF NOT EXISTS documents_embedding_hnsw
            ON documents USING hnsw (embedding vector_cosine_ops);
            """
        )
        conn.commit()
