import asyncio

from app.db import connect, ensure_schema, to_pgvector
from app.mistral_client import embed

INDICATEURS = [
    (
        "Selon l'INSEE, en moyenne sur l'année 2025, le taux de chômage en France "
        "au sens du BIT est de 7,7 % de la population active. Il s'agit d'une "
        "moyenne annuelle : les mesures mensuelles d'Eurostat peuvent différer. "
        "Source : INSEE, 2025.",
        "https://www.insee.fr/fr/statistiques/8735266",
        True,
    ),
    (
        "Selon l'INSEE, en 2025, le taux de chômage des jeunes de 15 à 24 ans en "
        "France est de 19,8 % en moyenne sur l'année (au sens du BIT). Eurostat "
        "publie le même indicateur pour les moins de 25 ans, en mensuel. "
        "Source : INSEE, 2025.",
        "https://www.insee.fr/fr/statistiques/8735266",
        True,
    ),
]



async def ingest() -> None:
    ensure_schema()
    with connect() as conn, conn.cursor() as cur:
        cur.execute("DELETE FROM documents WHERE source = 'data_gouv';")
        conn.commit()

    textes = [texte for texte, _, _ in INDICATEURS]
    vecteurs = await embed(textes)

    with connect() as conn, conn.cursor() as cur:
        for (texte, url, _verifie), vec in zip(INDICATEURS, vecteurs):
            cur.execute(
                "INSERT INTO documents (source, title, url, content, chunk_index, embedding) "
                "VALUES (%s, %s, %s, %s, %s, %s::vector)",
                ("data_gouv", "Indicateur officiel", url, texte, 0, to_pgvector(vec)),
            )
        conn.commit()

    non_verifies = sum(1 for _, _, v in INDICATEURS if not v)
    print(f"{len(INDICATEURS)} indicateurs ingérés (source='data_gouv').")
    if non_verifies:
        print(f"  ⚠️  {non_verifies} sont marqués verifie=False -> à confirmer avant la soutenance.")


if __name__ == "__main__":
    asyncio.run(ingest())
