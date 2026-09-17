import os
import re
import unicodedata
from pathlib import Path
from urllib.parse import quote

from app.db import connect

DATA = Path(__file__).resolve().parents[2] / "data"
TSV_FILES = sorted(DATA.glob("video_candidates*.tsv"))

YOUTUBE_RE = re.compile(
    r"https?://(?:www\.)?(?:youtube\.com/(?:watch\?v=|embed/|shorts/)|youtu\.be/)([\w-]{6,})",
    re.I,
)
FACEBOOK_RE = re.compile(
    r"https?://(?:www\.)?(?:facebook\.com/[\w.]+/videos/\d+|fb\.watch/[\w-]+)",
    re.I,
)

AFFIRMATION_RE = re.compile(r"Affirmation\s*:\s*«\s*(.+?)\s*»", re.S)
VERDICT_RE = re.compile(r"Verdict\s*:\s*(.+?)\s*—", re.S)


def _sans_accents(t: str) -> str:
    t = t.lower()
    t = unicodedata.normalize("NFD", t)
    return "".join(c for c in t if unicodedata.category(c) != "Mn")


AMBIGUS = ("moitie", "half", "mixture", "nuance", "plutot", "partiellement",
           "mostly", "en partie", "inverifiable", "unproven", "imprecis", "exagere")
FAUX = ("faux", "fausse", "fake", "false", "trompeur", "trompeuse", "manipule",
        "manipulee", "infonde", "intox", "canular", "detourne", "truque", "errone")
VRAI = ("vrai", "vraie", "true", "exact", "exacte", "correct", "avere")


def _label(verdict: str) -> str | None:
    v = _sans_accents(verdict).strip()
    if not v or len(v) > 40:
        return None
    if any(m in v for m in AMBIGUS):
        return None
    mots = set(re.findall(r"[a-z]+", v))
    if mots & set(FAUX):
        return "FALSE"
    if mots & set(VRAI):
        return "TRUE"
    return None


def _embed(blob: str) -> str | None:
    ids = YOUTUBE_RE.findall(blob)
    if ids:
        return f"https://www.youtube-nocookie.com/embed/{max(set(ids), key=ids.count)}"
    liens = FACEBOOK_RE.findall(blob)
    if liens:
        cible = max(set(liens), key=liens.count)
        return ("https://www.facebook.com/plugins/video.php?href="
                + quote(cible, safe="") + "&show_text=false")
    return None


def main() -> None:
    if not TSV_FILES:
        print(f"Aucun fichier de candidats dans {DATA} — lancer extract_videos.py.")
        return

    videos: dict[str, str] = {}
    for fichier in TSV_FILES:
        for ligne in fichier.read_text(encoding="utf-8").splitlines()[1:]:
            colonnes = ligne.split("\t")
            if len(colonnes) < 3:
                continue
            embed = _embed(" ".join(colonnes[2:]))
            if embed:
                videos[colonnes[1].strip().rstrip("/")] = embed
    print(f"{len(videos)} articles avec une vidéo embarquable "
          f"({len(TSV_FILES)} fichier(s) de candidats).")

    with connect() as conn, conn.cursor() as cur:
        cur.execute("SELECT url, content FROM documents WHERE source = 'factcheck';")
        documents = cur.fetchall()
        cur.execute('SELECT "sourceUrl" FROM "Claim" WHERE "sourceUrl" IS NOT NULL;')
        deja = {r[0].rstrip("/") for r in cur.fetchall()}

    exige_video = os.getenv("CLAIMS_REQUIRE_VIDEO", "1") != "0"

    nouvelles = []
    vus: set[str] = set()
    for url, contenu in documents:
        cle = (url or "").rstrip("/")
        if cle in deja or cle in vus:
            continue
        if exige_video and cle not in videos:
            continue
        affirmation = AFFIRMATION_RE.search(contenu)
        verdict = VERDICT_RE.search(contenu)
        if not affirmation or not verdict:
            continue
        label = _label(verdict.group(1))
        if label is None:
            continue
        texte = affirmation.group(1).strip()
        if not (10 <= len(texte) <= 1000):
            continue
        vus.add(cle)
        import hashlib
        cle_source = "video:" + hashlib.sha1(cle.encode()).hexdigest()[:16]
        nouvelles.append((cle_source, texte, label, "viral_debunk", url, videos.get(cle)))

    if not nouvelles:
        print("Aucune claim nouvelle à créer.")
        return

    import secrets
    import string

    alphabet = string.ascii_lowercase + string.digits
    avec_id = [
        ("c" + "".join(secrets.choice(alphabet) for _ in range(24)), *n) for n in nouvelles
    ]

    with connect() as conn, conn.cursor() as cur:
        cur.executemany(
            'INSERT INTO "Claim" (id, "sourceKey", text, "truthLabel", category, '
            '"sourceUrl", "mediaRef", "createdAt") '
            'VALUES (%s, %s, %s, %s::"ClaimTruthLabel", %s, %s, %s, now()) '
            'ON CONFLICT ("sourceKey") DO NOTHING',
            avec_id,
        )
        conn.commit()

    vrais = sum(1 for n in nouvelles if n[2] == "TRUE")
    print(f"{len(nouvelles)} claims créées avec vidéo ({vrais} vraies / {len(nouvelles) - vrais} fausses).")


if __name__ == "__main__":
    main()
