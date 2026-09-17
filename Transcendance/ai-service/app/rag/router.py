import json
import os

import httpx
from fastapi import APIRouter, HTTPException, Request
from fastapi.responses import StreamingResponse
from pydantic import BaseModel, Field

from app.mistral_client import chat_stream
from app.rag.guardrail import is_blocked
from app.rag.search import search_similar
from app.rate_limit import RateLimiter

router = APIRouter()

TOP_K = int(os.getenv("RAG_TOP_K", "6"))

_RATE_MAX = int(os.getenv("RAG_RATE_MAX", "10"))
_RATE_WINDOW = float(os.getenv("RAG_RATE_WINDOW", "30"))
limiter = RateLimiter(_RATE_MAX, _RATE_WINDOW)


class AskRequest(BaseModel):
    question: str = Field(min_length=3, max_length=1000)

    live_topics: list[str] = Field(default_factory=list, max_length=100)

    exclure_urls: list[str] = Field(default_factory=list, max_length=30)
    live_claims: list[str] = Field(default_factory=list, max_length=20)


def _build_messages(question: str, passages: list[dict]) -> list[dict]:
    contexte = "\n\n".join(
        f"[{i + 1}] {p['title']}\n{p['content']}" for i, p in enumerate(passages)
    )
    systeme = (
        "Tu es un assistant de fact-checking. Réponds EN FRANÇAIS, de façon concise "
        "et factuelle.\n"
        "Quand les passages fournis couvrent la question, appuie-toi dessus et "
        "renvoie-y explicitement.\n"
        "Quand ils ne la couvrent pas — c'est fréquent, le corpus ne contient que "
        "des vérifications et des statistiques — réponds quand même à partir de tes "
        "propres connaissances, mais commence par « Sans source dans mon corpus : » "
        "afin qu'on sache que cette réponse-là n'est pas vérifiée.\n"
        "N'invente jamais de source ni de citation."
    )
    return [
        {"role": "system", "content": systeme},
        {"role": "user", "content": f"Passages :\n\n{contexte}\n\nQuestion : {question}"},
    ]


def _sse(event: str, data) -> str:
    return f"event: {event}\ndata: {json.dumps(data, ensure_ascii=False)}\n\n"


def _message_erreur(erreur: Exception) -> str:
    if isinstance(erreur, httpx.HTTPStatusError) and erreur.response.status_code == 429:
        return "L'assistant a atteint la limite d'appels de l'IA. Il ne peut pas répondre pour le moment."
    return "La génération a été interrompue."


_REFUS = (
    "Je ne me prononce pas sur un clip de la manche en cours — ce serait te "
    "donner la réponse. En revanche je peux t'aider à la trouver : demande-moi "
    "comment reconnaître une vidéo générée par IA, un montage, ou une image "
    "authentique sortie de son contexte."
)


def _refusal_stream():
    yield _sse("sources", [])
    yield _sse("token", {"text": _REFUS})
    yield _sse("done", {})


class SecondAvisRequest(BaseModel):
    claim: str = Field(min_length=10, max_length=1000)

    exclure_url: str | None = Field(default=None, max_length=500)


CONSIGNE_SECOND_AVIS = (
    "Tu donnes ton avis sur une affirmation, comme le ferait un joueur attentif. "
    "Appuie-toi UNIQUEMENT sur les passages fournis. Si tu n'as pas d'élément, "
    "dis-le franchement plutôt que d'inventer.\n"
    "Réponds exactement ainsi :\n"
    "AVIS: VRAI | FAUX | INCERTAIN\n"
    "POURQUOI: <une seule phrase>"
)


@router.post("/second-avis")
async def second_avis(req: SecondAvisRequest, request: Request):
    if not limiter.allow(request.client.host):
        raise HTTPException(status_code=429, detail="Trop de requêtes.")

    try:
        passages = await search_similar(req.claim, k=TOP_K + 4)
    except Exception:
        raise HTTPException(status_code=503, detail="Service RAG indisponible (recherche).")

    if req.exclure_url:
        cible = req.exclure_url.rstrip("/")
        passages = [p for p in passages if (p.get("url") or "").rstrip("/") != cible]
    passages = passages[:TOP_K]

    contexte = "\n\n".join(f"[{i + 1}] {p['content']}" for i, p in enumerate(passages))
    messages = [
        {"role": "system", "content": CONSIGNE_SECOND_AVIS},
        {"role": "user", "content": f"Passages :\n\n{contexte}\n\nAffirmation : « {req.claim} »"},
    ]

    try:
        morceaux = [m async for m in chat_stream(messages)]
    except Exception:
        raise HTTPException(status_code=503, detail="Service LLM indisponible.")
    texte = "".join(morceaux).strip()

    avis = "INCERTAIN"
    for candidat in ("VRAI", "FAUX", "INCERTAIN"):
        if f"AVIS: {candidat}" in texte.upper():
            avis = candidat
            break
    pourquoi = texte.split("POURQUOI:", 1)[-1].strip() if "POURQUOI:" in texte else texte

    return {
        "avis": avis,
        "pourquoi": pourquoi,
        "citations": [{"title": p["title"], "url": p["url"]} for p in passages],
    }


@router.post("/ask")
async def ask(req: AskRequest, request: Request):
    if not limiter.allow(request.client.host):
        raise HTTPException(
            status_code=429,
            detail=f"Trop de requêtes (max {_RATE_MAX} par {int(_RATE_WINDOW)}s). Réessayez dans un instant.",
        )

    if req.live_topics or req.live_claims:
        try:
            blocked = await is_blocked(req.question, req.live_topics, req.live_claims)
        except Exception:
            raise HTTPException(status_code=503, detail="Garde-fou indisponible, réessayez.")
        if blocked:
            return StreamingResponse(_refusal_stream(), media_type="text/event-stream")

    marge = TOP_K + 4 if req.exclure_urls else TOP_K
    try:
        passages = await search_similar(req.question, k=marge)
    except Exception:
        raise HTTPException(status_code=503, detail="Service RAG indisponible (recherche).")

    if req.exclure_urls:
        interdits = {u.rstrip("/") for u in req.exclure_urls}
        passages = [p for p in passages if (p.get("url") or "").rstrip("/") not in interdits]
    passages = passages[:TOP_K]

    en_manche = bool(req.live_claims or req.live_topics)
    messages = _build_messages(req.question, passages)
    if en_manche and messages and messages[0].get("role") == "system":
        messages[0] = {
            **messages[0],
            "content": messages[0]["content"]
            + "\n\nUne partie est en cours. Explique le sujet, le contexte et les "
            "techniques de vérification, mais ne dis JAMAIS si une vidéo précise "
            "est authentique ou truquée, même si les passages le laissent penser.",
        }

    async def event_stream():
        vues: set[str] = set()
        sources = []
        for p in passages:
            if p["url"] in vues:
                continue
            vues.add(p["url"])
            sources.append({"title": p["title"], "url": p["url"]})
        yield _sse("sources", sources)
        try:
            async for token in chat_stream(messages):
                yield _sse("token", {"text": token})
        except Exception as erreur:
            yield _sse("error", {"message": _message_erreur(erreur)})
            return
        yield _sse("done", {})

    return StreamingResponse(event_stream(), media_type="text/event-stream")
