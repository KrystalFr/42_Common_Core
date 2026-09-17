import hashlib
import re
import secrets
import string
import unicodedata

from app.db import connect

DETOURNEMENTS = [
    ("tornado",
     "Cette vidéo montre la tornade qui a frappé la Normandie en octobre 2018."),
    ("quake|earthquake",
     "Cette vidéo montre les secousses du séisme de Turquie de février 2023."),
    ("eclipse",
     "Cette vidéo montre l'éclipse solaire totale observée depuis la France en 1999."),
    ("volcano|eruption|erupting",
     "Cette vidéo montre l'éruption du Vésuve filmée par un touriste."),
    ("geyser",
     "Cette vidéo montre un geyser apparu dans un jardin après un forage en Islande."),
    ("avalanche",
     "Cette vidéo montre l'avalanche qui a emporté un refuge dans les Alpes françaises."),
    ("balen|whale",
     "Cette vidéo montre une baleine remontée jusque dans la Seine à Rouen."),
]


def _sans_accents(t: str) -> str:
    t = t.lower()
    t = unicodedata.normalize("NFD", t)
    return "".join(c for c in t if unicodedata.category(c) != "Mn")


def _sujet_reel(media: str) -> str:
    """Sujet véritable de la vidéo, lu dans le nom du fichier Commons."""
    nom = re.sub(r"\.(webm|ogv|mp4)(\?.*)?$", "", media.split("/")[-1])
    return nom.replace("_", " ")[:120]


def main() -> None:
    with connect() as conn, conn.cursor() as cur:
        cur.execute(
            'SELECT "mediaRef", "sourceUrl" FROM "Claim" '
            'WHERE "mediaRef" LIKE %s AND "truthLabel" = %s',
            ("%wikimedia%", "TRUE"),
        )
        sources = cur.fetchall()
        cur.execute('SELECT "sourceKey" FROM "Claim" WHERE "sourceKey" IS NOT NULL')
        deja = {r[0] for r in cur.fetchall()}

    nouvelles = []
    utilises: set[str] = set()
    for media, source_url in sources:
        fichier = _sans_accents(_sujet_reel(media))
        for motif, fausse_attribution in DETOURNEMENTS:
            if not re.search(motif, fichier, re.I):
                continue
            if motif in utilises:
                break
            utilises.add(motif)
            cle = "horscontexte:" + hashlib.sha1(media.encode()).hexdigest()[:16]
            if cle in deja:
                break
            nouvelles.append((cle, fausse_attribution, "FALSE", "hors_contexte",
                              source_url, media))
            break

    if not nouvelles:
        print("Aucune claim hors-contexte à créer.")
        return

    alphabet = string.ascii_lowercase + string.digits
    avec_id = [("c" + "".join(secrets.choice(alphabet) for _ in range(24)), *n) for n in nouvelles]

    with connect() as conn, conn.cursor() as cur:
        cur.executemany(
            'INSERT INTO "Claim" (id, "sourceKey", text, "truthLabel", category, '
            '"sourceUrl", "mediaRef", "createdAt") '
            'VALUES (%s, %s, %s, %s::"ClaimTruthLabel", %s, %s, %s, now()) '
            'ON CONFLICT ("sourceKey") DO NOTHING',
            avec_id,
        )
        conn.commit()

    print(f"{len(nouvelles)} claims HORS CONTEXTE créées :\n")
    for _cle, texte, _lab, _cat, _src, media in nouvelles:
        print(f"  « {texte[:66]} »")
        print(f"    (en réalité : {_sujet_reel(media)[:64]})")


if __name__ == "__main__":
    main()
