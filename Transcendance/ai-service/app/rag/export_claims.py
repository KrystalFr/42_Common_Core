import hashlib
import json
import os
import re
import time
import unicodedata
from pathlib import Path

import httpx

from app.config import settings

API = "https://factchecktools.googleapis.com/v1alpha1/claims:search"

QUERIES = {
    "vidéo": "viral_debunk",
    "intelligence artificielle": "viral_debunk",
    "élection": "politique_fr",
    "gouvernement": "politique_fr",
    "immigration": "politique_fr",
    "climat": "science",
    "vaccin": "sante",
    "santé": "sante",
}
MAX_AGE_DAYS = int(os.getenv("CLAIMS_MAX_AGE_DAYS", "1825"))
PAGES_PER_QUERY = int(os.getenv("CLAIMS_PAGES", "4"))

OUT = Path(__file__).resolve().parents[2] / "data" / "editorial-claims.generated.json"

FALSE_MARKERS = (
    "faux", "fausse", "fausses", "fake", "false", "incorrect", "incorrecte",
    "errone", "erronee", "trompeur", "trompeuse", "misleading", "infonde",
    "infondee", "sans fondement", "intox", "hoax", "canular", "manipule",
    "manipulee", "altered", "hors contexte", "out of context", "decontextualise",
    "satire", "parodie", "invente", "inventee", "mensonger", "mensongere",
    "pants on fire", "scam", "arnaque", "detourne", "detournee", "truque", "truquee",
)
TRUE_MARKERS = (
    "vrai", "vraie", "true", "correct", "correcte", "exact", "exacte",
    "avere", "averee", "accurate",
)
MAX_RATING_LEN = 40
AMBIGUOUS_MARKERS = (
    "moitie", "half", "mixture", "mixte", "nuance", "a nuancer", "plutot",
    "partiellement", "partly", "mostly", "en partie", "invérifiable",
    "inverifiable", "unproven", "non prouve", "indetermine", "manque de contexte",
    "imprecis", "exagere", "depend",
)


def _strip(text: str) -> str:
    """Minuscule sans accents — pour comparer des verdicts écrits librement."""
    text = text.lower()
    text = unicodedata.normalize("NFD", text)
    return "".join(c for c in text if unicodedata.category(c) != "Mn")


def map_verdict(rating: str) -> str | None:
    """Verdict libre du fact-checkeur -> 'TRUE' / 'FALSE' / None (= à jeter).

    L'ordre compte : on écarte d'abord l'ambigu, sinon « plutôt faux » serait
    capté par le marqueur « faux » alors que le fact-checkeur a justement nuancé.
    """
    r = _strip(rating or "").strip()
    if not r or len(r) > MAX_RATING_LEN:
        return None
    if any(m in r for m in AMBIGUOUS_MARKERS):
        return None
    mots = set(re.findall(r"[a-z]+", r))
    if mots & set(FALSE_MARKERS) or any(m in r for m in FALSE_MARKERS if " " in m):
        return "FALSE"
    if mots & set(TRUE_MARKERS):
        return "TRUE"
    return None


def make_source_key(publisher: str, url: str) -> str:
    """Clé stable et rejouable, au format exigé par l'importeur du Back Core.

    Motif imposé : ^[a-z0-9][a-z0-9:._-]{2,127}$. On préfixe par l'éditeur (pour
    la lisibilité en base) et on suffixe par un hash de l'URL (pour l'unicité et
    l'idempotence : réimporter le même article ne crée pas de doublon).
    """
    slug = re.sub(r"[^a-z0-9]+", "-", _strip(publisher)).strip("-")[:40] or "factcheck"
    digest = hashlib.sha1(url.encode()).hexdigest()[:12]
    key = f"{slug}:{digest}"
    return key if re.match(r"^[a-z0-9][a-z0-9:._-]{2,127}$", key) else f"fc:{digest}"


STOP = {
    "dans", "pour", "avec", "sans", "sur", "sous", "chez", "vers", "cette", "cet",
    "les", "des", "une", "aux", "que", "qui", "quoi", "dont", "est", "sont", "ete",
    "avoir", "etre", "plus", "moins", "tres", "tout", "tous", "toute", "toutes",
    "leur", "leurs", "nous", "vous", "ils", "elles", "son", "sa", "ses", "par",
    "pas", "ne", "en", "au", "du", "de", "la", "le", "un", "et", "ou", "il", "elle",
}


def theme_tags(text: str, limit: int = 6) -> list[str]:
    """Mots-thèmes CANDIDATS pour le garde-fou anti-spoil (cf. guardrail.py).

    ⚠️ Ce sont des candidats automatiques, à RELIRE : docs/CLAIMS.md prévoit une
    curation humaine. On prend les mots longs et rares, hors mots vides. Ce champ
    est ignoré par l'importeur actuel (la colonne themeTags n'existe pas encore) :
    il sert à préparer le terrain.
    """
    mots = re.findall(r"[a-zA-ZÀ-ÿ']{4,}", text)
    vus, out = set(), []
    for m in mots:
        n = _strip(m)
        if n in STOP or n in vus:
            continue
        vus.add(n)
        out.append(n)
        if len(out) >= limit:
            break
    return out


def _get(client: httpx.Client, params: dict) -> dict:
    """Appel API avec réessais sur 429 (quota) et pépins réseau."""
    for attempt in range(3):
        try:
            resp = client.get(API, params=params, timeout=30)
            resp.raise_for_status()
            return resp.json()
        except httpx.HTTPStatusError as e:
            if e.response.status_code != 429 or attempt == 2:
                raise
            time.sleep(2 ** attempt)
        except (httpx.TransportError, json.JSONDecodeError):
            if attempt == 2:
                raise
            time.sleep(2 ** attempt)
    return {}


def collect() -> tuple[list[dict], dict]:
    """Interroge l'API pour chaque thème et renvoie (claims valides, statistiques)."""
    seen: set[str] = set()
    claims: list[dict] = []
    stats = {"vus": 0, "verdict_ambigu": 0, "url_non_https": 0, "texte_hors_bornes": 0, "doublon": 0}

    with httpx.Client() as client:
        for query, category in QUERIES.items():
            page_token = None
            for _ in range(PAGES_PER_QUERY):
                params = {
                    "query": query, "languageCode": "fr",
                    "maxAgeDays": MAX_AGE_DAYS, "pageSize": 50,
                    "key": settings.google_api_key,
                }
                if page_token:
                    params["pageToken"] = page_token
                data = _get(client, params)

                for claim in data.get("claims", []):
                    stats["vus"] += 1
                    reviews = claim.get("claimReview") or []
                    if not reviews:
                        continue
                    review = reviews[0]
                    url = (review.get("url") or "").strip()
                    text = (claim.get("text") or "").strip()

                    label = map_verdict(review.get("textualRating", ""))
                    if label is None:
                        stats["verdict_ambigu"] += 1
                        continue
                    if not url.startswith("https://"):
                        stats["url_non_https"] += 1
                        continue
                    if not (10 <= len(text) <= 1000):
                        stats["texte_hors_bornes"] += 1
                        continue

                    key = make_source_key((review.get("publisher") or {}).get("name", ""), url)
                    if key in seen:
                        stats["doublon"] += 1
                        continue
                    seen.add(key)

                    claims.append({
                        "sourceKey": key,
                        "text": text,
                        "truthLabel": label,
                        "category": category,
                        "sourceUrl": url,
                        "_publisher": (review.get("publisher") or {}).get("name", ""),
                        "_rating": review.get("textualRating", ""),
                        "_reviewDate": (review.get("reviewDate") or "")[:10],
                        "themeTags": theme_tags(text),
                    })

                page_token = data.get("nextPageToken")
                if not page_token:
                    break
    return claims, stats


def main() -> None:
    if not settings.google_api_key:
        print("GOOGLE_API_KEY manquante dans .env — impossible d'interroger l'API.")
        return

    print("Interrogation de l'API Google Fact Check...")
    claims, stats = collect()

    if not claims:
        print("Aucune claim exploitable récupérée.")
        return

    vrais_l = [c for c in claims if c["truthLabel"] == "TRUE"]
    faux_l = [c for c in claims if c["truthLabel"] == "FALSE"]
    total_brut = len(claims)
    if vrais_l:
        faux_l = faux_l[: int(len(vrais_l) * 1.5)]
    claims = vrais_l + faux_l

    OUT.parent.mkdir(parents=True, exist_ok=True)
    OUT.write_text(json.dumps(claims, ensure_ascii=False, indent=2), encoding="utf-8")

    vrais = len(vrais_l)
    faux = len(faux_l)
    print(f"  (équilibrage : {total_brut} claims collectées -> {len(claims)} retenues)")
    print(f"\n{len(claims)} claims écrites dans {OUT}")
    print(f"  répartition : {vrais} TRUE / {faux} FALSE")
    print(f"  par catégorie : ", {c: sum(1 for x in claims if x['category'] == c) for c in set(x['category'] for x in claims)})
    print(f"  écartées : {stats}")
    print("\nCôté Back Core, pour importer :")
    print("  cp ai-service/data/editorial-claims.generated.json backend/prisma/data/editorial-claims.json")
    print("  docker compose exec backend npm run claims:import")


if __name__ == "__main__":
    main()
