import os
import re
import secrets
import string
import time
import unicodedata

import httpx

from app.db import connect
from app.rag.export_claims import API, make_source_key, map_verdict
from app.rag.videos_depuis_factchecks import ENTETES, _embed_facebook, _extraire

REQUETES_FR = [
    "vidéo animal", "vidéo sauvetage", "vidéo tempête", "vidéo inondation",
    "vidéo accident", "vidéo foule", "vidéo police", "vidéo incendie",
    "vidéo requin", "vidéo ours", "vidéo neige", "vidéo avion",
    "vidéo insolite", "vidéo manifestation", "vidéo explosion", "vidéo météo",
    "vidéo chien", "vidéo chat", "vidéo oiseau", "vidéo serpent",
    "vidéo train", "vidéo métro", "vidéo pont", "vidéo immeuble",
    "vidéo grêle", "vidéo vague", "vidéo éclair", "vidéo glissement de terrain",
    "vidéo stade", "vidéo concert", "vidéo mariage", "vidéo marché",
]
REQUETES_EN = [
    "video animal", "video rescue", "video storm", "video flood",
    "video accident", "video crowd", "video police", "video fire",
    "video shark", "video bear", "video snow", "video plane",
    "video lightning", "video wave", "video explosion", "video tornado",
    "video whale", "video crocodile", "video landslide", "video iceberg",
    "video dog", "video cat", "video bird", "video snake",
    "video elephant", "video monkey", "video spider", "video insect",
    "video train", "video bridge", "video building", "video crane",
    "video hail", "video sinkhole", "video volcano", "video earthquake",
    "video stadium", "video concert", "video wedding", "video market",
    "video truck", "video boat", "video helicopter", "video drone",
    "video escalator", "video elevator", "video pool", "video beach",
]

MOTS_VIDEO = ("vidéo", "video", "séquence", "footage", "clip", "images")
MOTS_IMAGE = ("photograph", "photographie", "screenshot", "capture d'écran", "a photo")

VIDES = {
    "cette", "video", "vidéo", "montre", "dans", "avec", "pour", "leur", "elle",
    "shows", "video", "this", "that", "with", "from", "have", "been", "were",
    "authentically", "authentic", "real", "genuine", "footage", "president",
}


def _normaliser(texte: str) -> set[str]:
    sans_accent = "".join(
        c for c in unicodedata.normalize("NFD", texte.lower())
        if unicodedata.category(c) != "Mn"
    )
    return {m for m in re.findall(r"[a-z]{4,}", sans_accent) if m not in VIDES}


def _titre_trahit(titre: str, affirmation: str) -> bool:
    """Le titre de la vidéo révèle-t-il la réponse ?

    On compare les mots significatifs. Trois mots communs suffisent : à ce
    niveau le titre raconte la même chose que l'énoncé, et le joueur n'a plus
    qu'à lire au lieu de juger l'image.
    """
    communs = _normaliser(titre) & _normaliser(affirmation)
    return len(communs) >= 3


def _infos_youtube(video_id: str) -> str | None:
    for _essai in range(3):
        try:
            r = httpx.get("https://www.youtube.com/oembed",
                          params={"url": f"https://www.youtube.com/watch?v={video_id}",
                                  "format": "json"},
                          headers=ENTETES, timeout=25)
            if r.status_code == 200:
                return r.json().get("title", "")
            if r.status_code in (401, 403, 404):
                return None
        except Exception:
            pass
        time.sleep(3)
    return None


def _media_utilisable(html: str, affirmation: str) -> str | None:
    """Vidéo à retenir pour cette affirmation, Facebook d'abord."""
    for brut in re.findall(
        r'https?://(?:www\.|web\.|m\.)?facebook\.com/[^"\'<>\s\\]+?/videos/[0-9A-Za-z._-]+', html
    ):
        return _embed_facebook(brut.replace("\\/", "/").rstrip('\\"\''))

    media = _extraire(html)
    if not media or "/embed/" not in media:
        return None
    identifiant = re.search(r"/embed/([A-Za-z0-9_-]{11})", media)
    if not identifiant:
        return None
    titre = _infos_youtube(identifiant.group(1))
    time.sleep(1)
    if titre is None or _titre_trahit(titre, affirmation):
        return None
    return media


def main() -> None:
    cle = os.getenv("GOOGLE_API_KEY")
    if not cle:
        print("GOOGLE_API_KEY manquante dans .env.")
        return

    candidats: dict[str, dict] = {}
    for langue, requetes in (("fr", REQUETES_FR), ("en", REQUETES_EN)):
        for requete in requetes:
            for page in range(3):
                try:
                    r = httpx.get(API, params={"key": cle, "query": requete,
                                               "languageCode": langue, "pageSize": 50,
                                               "offset": page * 50}, timeout=40)
                except Exception:
                    break
                if r.status_code != 200:
                    break
                claims = r.json().get("claims", [])
                if not claims:
                    break
                for c in claims:
                    texte = (c.get("text") or "").strip()
                    bas = texte.lower()
                    if not (30 <= len(texte) <= 240):
                        continue
                    if not any(m in bas for m in MOTS_VIDEO):
                        continue
                    if any(m in bas for m in MOTS_IMAGE):
                        continue
                    for rev in c.get("claimReview", []):
                        if map_verdict(rev.get("textualRating", "")) != "TRUE":
                            continue
                        url = rev.get("url")
                        if not url:
                            continue
                        candidats.setdefault(texte, {
                            "texte": texte, "url": url,
                            "editeur": rev.get("publisher", {}).get("name", "factcheck"),
                        })
                time.sleep(1)

    print(f"{len(candidats)} vidéo(s) confirmée(s) trouvée(s).\n")
    if not candidats:
        return

    with connect() as conn, conn.cursor() as cur:
        cur.execute('SELECT "sourceKey" FROM "Claim" WHERE "sourceKey" IS NOT NULL')
        deja = {r[0] for r in cur.fetchall()}

    alphabet = string.ascii_lowercase + string.digits
    lignes, sans_video = [], 0
    for donnees in candidats.values():
        cle_source = make_source_key(donnees["editeur"], donnees["url"])
        if cle_source in deja:
            continue
        try:
            page = httpx.get(donnees["url"], headers=ENTETES, timeout=30,
                             follow_redirects=True)
            html = page.text if page.status_code == 200 else ""
        except Exception:
            html = ""
        time.sleep(1.5)
        if not html:
            sans_video += 1
            continue
        media = _media_utilisable(html, donnees["texte"])
        if not media:
            sans_video += 1
            continue
        lignes.append(("c" + "".join(secrets.choice(alphabet) for _ in range(24)),
                       cle_source, donnees["texte"], "TRUE",
                       "confirme_viral", donnees["url"], media))

    if lignes:
        with connect() as conn, conn.cursor() as cur:
            cur.executemany(
                'INSERT INTO "Claim" (id, "sourceKey", text, "truthLabel", category, '
                '"sourceUrl", "mediaRef", "createdAt") '
                'VALUES (%s, %s, %s, %s::"ClaimTruthLabel", %s, %s, %s, now()) '
                'ON CONFLICT ("sourceKey") DO NOTHING',
                lignes,
            )
            conn.commit()

    facebook = sum(1 for l in lignes if "facebook" in l[6])
    print(f"{len(lignes)} clip(s) VRAIS insérés — {facebook} Facebook, "
          f"{len(lignes) - facebook} YouTube. {sans_video} sans vidéo exploitable.\n")
    for ligne in lignes:
        source = "facebook" if "facebook" in ligne[6] else "youtube"
        print(f"  [{source:8s}] {ligne[2][:80]}")


if __name__ == "__main__":
    main()
