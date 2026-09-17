import json
import os

from fastapi import APIRouter, HTTPException, Request
from fastapi.responses import StreamingResponse
from pydantic import BaseModel, Field

from app.mistral_client import chat_stream
from app.rate_limit import RateLimiter

router = APIRouter()

_RATE_MAX = int(os.getenv("LLM_RATE_MAX", "5"))
_RATE_WINDOW = float(os.getenv("LLM_RATE_WINDOW", "30"))
limiter = RateLimiter(_RATE_MAX, _RATE_WINDOW)


class ChatRequest(BaseModel):
    message: str = Field(min_length=1, max_length=4000)


def _sse(event: str, data) -> str:
    return f"event: {event}\ndata: {json.dumps(data, ensure_ascii=False)}\n\n"


@router.post("/chat")
async def chat(req: ChatRequest, request: Request):
    if not limiter.allow(request.client.host):
        raise HTTPException(
            status_code=429,
            detail=f"Trop de requêtes (max {_RATE_MAX} par {int(_RATE_WINDOW)}s). Réessayez dans un instant.",
        )

    messages = [
        {"role": "system", "content": "Tu es un assistant francophone. Réponds de façon claire et concise."},
        {"role": "user", "content": req.message},
    ]

    async def event_stream():
        try:
            async for token in chat_stream(messages):
                yield _sse("token", {"text": token})
        except Exception:
            yield _sse("error", {"message": "La génération a été interrompue."})
            return
        yield _sse("done", {})

    return StreamingResponse(event_stream(), media_type="text/event-stream")
