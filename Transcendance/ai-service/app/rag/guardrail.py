import re
import unicodedata


def _normalize(text: str) -> str:
    text = text.lower()
    text = unicodedata.normalize("NFD", text)
    return "".join(c for c in text if unicodedata.category(c) != "Mn")


def is_about_live_topics(question: str, live_topics: list[str]) -> bool:
    if not live_topics:
        return False
    q = _normalize(question)
    for topic in live_topics:
        t = _normalize(topic).strip()
        if not t:
            continue
        if " " in t:
            if t in q:  # expression multi-mots -> sous-chaîne
                return True
        elif re.search(rf"\b{re.escape(t)}\b", q):  # mot simple -> mot entier
            return True
    return False


async def judged_about_live_clips(question: str, live_claims: list[str]) -> bool:
    from app.mistral_client import chat_stream

    bloc = "\n".join(f"- {c}" for c in live_claims)
    messages = [
        {
            "role": "system",
            "content": (
                "Tu es un filtre anti-spoil pour un jeu de fact-checking. On te donne "
                "les affirmations d'une manche EN COURS. Réponds UNIQUEMENT par OUI ou "
                "par NON, rien d'autre.\n"
                "OUI seulement si la question demande le VERDICT de l'une de ces "
                "affirmations : si elle est vraie, fausse, authentique, truquée, ou si "
                "elle reprend l'affirmation pour la faire confirmer ou infirmer.\n"
                "NON pour tout le reste, y compris quand la question porte sur le même "
                "thème : demander des connaissances générales, du contexte historique, "
                "ou comment vérifier ce genre de vidéo n'est PAS un spoil — c'est même "
                "le but du jeu. En cas de doute sur cette distinction, réponds NON."
            ),
        },
        {
            "role": "user",
            "content": f"Affirmations de la manche :\n{bloc}\n\nQuestion : {question}",
        },
    ]
    parts = [token async for token in chat_stream(messages)]
    return "oui" in _normalize("".join(parts))


_DEMANDE_DE_VERDICT = re.compile(
    r"\b(clip|vid[ée]o|extrait|affirmation|manche)\b[^?]{0,80}"
    r"\b(vrai|faux|authentique|truqu\w*|fake|r[ée]ponse|verdict|r[ée]el\w*)\b"
    r"|\b(vrai|faux|authentique|truqu\w*|fake|r[ée]ponse|verdict)\b[^?]{0,80}"
    r"\b(clip|vid[ée]o|extrait|affirmation|manche)\b",
    re.IGNORECASE,
)


async def is_blocked(
    question: str,
    live_topics: list[str],
    live_claims: list[str],
) -> bool:
    if not live_claims:
        return False
    if _DEMANDE_DE_VERDICT.search(question):
        return True
    return await judged_about_live_clips(question, live_claims)


if __name__ == "__main__":
    live = ["aigle", "bébé", "combat", "guerre", "Arma", "chanteuse", "Sahel"]
    questions = [
        "l'aigle qui attrape le bébé c'est réel ?",        # touche « aigle », « bébé »
        "est-ce que le combat de nuit est authentique ?",  # touche « combat »
        "c'est quoi la capitale de l'Australie ?",         # aucun thème
        "comment fonctionne un moteur de recherche ?",     # aucun thème
    ]
    for q in questions:
        flag = "REFUS" if is_about_live_topics(q, live) else "ok"
        print(f"  [{flag:5}] «{q}»")
