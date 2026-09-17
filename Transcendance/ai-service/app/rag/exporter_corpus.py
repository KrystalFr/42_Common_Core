import json
import pathlib

from app.db import connect

SORTIE = pathlib.Path("/app/export/editorial-claims.generated.json")

JOUABLE = """("mediaRef" LIKE '%upload.wikimedia%'
              OR "mediaRef" LIKE '%/embed/%'
              OR "mediaRef" LIKE '%plugins/video.php%')"""


def main() -> None:
    with connect() as conn, conn.cursor() as cur:
        cur.execute(f'''
            SELECT cl."sourceKey", cl.text, cl."truthLabel", cl.category,
                   cl."sourceUrl", cl."mediaRef", cl."videoEndS",
                   v."verdictText", v.confidence, v.citations
            FROM "Claim" cl
            LEFT JOIN "ClaimVerdict" v ON v."claimId" = cl.id
            WHERE {JOUABLE}
            ORDER BY cl.category, cl.text
        ''')
        lignes = cur.fetchall()

    claims = []
    for cle, texte, label, categorie, source, media, fin, verdict, confiance, citations in lignes:
        entree = {
            "sourceKey": cle,
            "text": texte,
            "truthLabel": label,
            "category": categorie,
            "sourceUrl": source,
            "mediaRef": media,
        }
        if fin is not None:
            entree["videoEndS"] = fin
        if verdict:
            entree["verdict"] = {
                "verdictText": verdict,
                "confidence": confiance,
                "citations": citations or [],
            }
        claims.append(entree)

    SORTIE.parent.mkdir(parents=True, exist_ok=True)
    SORTIE.write_text(json.dumps(claims, ensure_ascii=False, indent=2) + "\n")

    vrais = sum(1 for c in claims if c["truthLabel"] == "TRUE")
    avec_explication = sum(1 for c in claims if "verdict" in c)
    print(f"{len(claims)} clips exportés vers {SORTIE}")
    print(f"  {vrais} vrais / {len(claims) - vrais} faux, {avec_explication} avec explication")


if __name__ == "__main__":
    main()
