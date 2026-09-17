import re
import unicodedata


def _sans_accents(t: str) -> str:
    t = t.lower()
    t = unicodedata.normalize("NFD", t)
    return "".join(c for c in t if unicodedata.category(c) != "Mn")


MARQUEURS_VRAI = re.compile(
    r"\b(veritable\w*|vraie?s?|authentiq\w+|reelle?s?|bien reel\w*|avere\w*)\b"
)
MARQUEURS_FAUX = re.compile(
    r"\b(fausse?s?|faux|truqu\w+|deepfake|manipul\w+|montage|canular|intox|"
    r"generee? par (l')?ia|arnaque|mise en scene|pretendu\w*|soi-disant)\b"
)
MARQUEURS_VERIFICATEUR = re.compile(r"\b(prouve\w*|demontre\w*|dementi\w*|contrairement a)\b")

LONGUEUR_MIN = 25
LONGUEUR_MAX = 300


def probleme(texte: str, label: str) -> str | None:
    brut = texte.strip()
    if len(brut) < LONGUEUR_MIN:
        return "trop court pour être compris hors contexte"
    if len(brut) > LONGUEUR_MAX:
        return "trop long pour être lu en jeu"
    if brut.endswith("?"):
        return "formulée en question, pas en affirmation"

    normalise = _sans_accents(brut)
    if label == "TRUE" and MARQUEURS_VRAI.search(normalise):
        return "annonce qu'elle est vraie"
    if label == "FALSE" and MARQUEURS_FAUX.search(normalise):
        return "annonce qu'elle est fausse"
    if MARQUEURS_VERIFICATEUR.search(normalise):
        return "reprend le point de vue du vérificateur"
    return None


def filtrer(claims, index_texte: int, index_label: int):
    gardees, rejetees = [], []
    for claim in claims:
        raison = probleme(claim[index_texte], claim[index_label])
        if raison:
            rejetees.append((claim, raison))
        else:
            gardees.append(claim)
    return gardees, rejetees
