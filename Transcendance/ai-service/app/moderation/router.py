import asyncio

from fastapi import APIRouter, HTTPException
from pydantic import BaseModel, Field

from app.mistral_client import chat_stream, moderate

router = APIRouter()

SEVERE_CATEGORIES = {
    "hate_and_discrimination",
    "violence_and_threats",
    "sexual",
    "dangerous_and_criminal_content",
    "selfharm",
}

SEUIL_SUPPRESSION = 0.05


DELAI_JUGE_S = 3.0


async def _juge_grossierete(message: str) -> bool | None:
    messages = [
        {
            "role": "system",
            "content": (
                "Tu modères le chat d'un jeu de fact-checking entre joueurs. "
                "Réponds UNIQUEMENT par OUI ou par NON, rien d'autre.\n"
                "OUI si le message contient une insulte, une attaque personnelle, "
                "ou un terme du registre grossier français — y compris employé "
                "seul, sans destinataire, ou sous forme d'abréviation.\n"
                "NON pour tout le reste. En particulier, le vocabulaire dur d'un "
                "sujet d'actualité (guerre, morts, blessés, attentat, drogue) "
                "n'est PAS de la grossièreté : les extraits vérifiés dans ce jeu "
                "en parlent constamment, et les censurer viderait le chat de son "
                "objet. En cas de doute sur cette distinction, réponds NON."
            ),
        },
        {"role": "user", "content": message},
    ]

    async def _demander() -> str:
        return "".join([token async for token in chat_stream(messages)])

    try:
        reponse = await asyncio.wait_for(_demander(), timeout=DELAI_JUGE_S)
    except Exception:
        return None
    return "oui" in reponse.strip().lower()


def _appliquer_juge(decision: dict, grossier: bool | None) -> dict:
    fusion = {**decision, "juge_llm": grossier}
    if grossier:
        fusion["action"] = "delete"
    return fusion


class ModerationRequest(BaseModel):
    message: str = Field(min_length=1, max_length=4000)
    history: list[str] = Field(default_factory=list, max_length=20)


def _decide(result: dict) -> dict:
    categories = result.get("categories", {})
    scores = result.get("category_scores", {})
    flagged = [name for name, hit in categories.items() if hit]

    au_dessus_du_seuil = [
        name for name in SEVERE_CATEGORIES
        if name not in flagged and float(scores.get(name, 0)) >= SEUIL_SUPPRESSION
    ]

    if any(name in SEVERE_CATEGORIES for name in flagged) or au_dessus_du_seuil:
        action = "delete"
    elif flagged:
        action = "warn"
    else:
        action = "allow"

    return {
        "action": action,
        "flagged": flagged + au_dessus_du_seuil,
        "au_dessus_du_seuil": au_dessus_du_seuil,
        "scores": scores,
    }


@router.post("/check")
async def check(req: ModerationRequest):
    try:
        context = "\n".join(req.history[-20:])
        moderated_input = f"Historique récent:\n{context}\n\nMessage à modérer:\n{req.message}" if context else req.message
        result = await moderate(moderated_input)
    except Exception:
        raise HTTPException(status_code=503, detail="Service de modération indisponible.")

    decision = _decide(result)
    if decision["action"] != "allow":
        return decision
    return _appliquer_juge(decision, await _juge_grossierete(req.message))


