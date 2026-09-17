from typing import AsyncIterator
import asyncio
import json

import httpx

from app.config import settings


def _headers() -> dict:
    return {
        "Authorization": f"Bearer {settings.llm_api_key}",
        "Content-Type": "application/json",
    }


_MAX_RETRIES = 5
_RETRYABLE_STATUS = {429, 500, 502, 503, 504}


async def _post_json(url: str, payload: dict, timeout: float) -> httpx.Response:
    for attempt in range(_MAX_RETRIES):
        try:
            async with httpx.AsyncClient(timeout=timeout) as client:
                resp = await client.post(url, headers=_headers(), json=payload)
            resp.raise_for_status()
            return resp
        except httpx.HTTPStatusError as e:
            if e.response.status_code not in _RETRYABLE_STATUS or attempt == _MAX_RETRIES - 1:
                raise
        except httpx.TransportError:
            if attempt == _MAX_RETRIES - 1:
                raise
        await asyncio.sleep(2 ** attempt)


async def embed(texts: list[str]) -> list[list[float]]:
    resp = await _post_json(
        f"{settings.mistral_api_base}/embeddings",
        {"model": settings.embedding_model, "input": texts},
        timeout=60,
    )
    data = resp.json()["data"]
    data.sort(key=lambda d: d["index"])
    return [d["embedding"] for d in data]


async def chat_stream(messages: list[dict]) -> AsyncIterator[str]:
    payload = {"model": settings.llm_model, "messages": messages, "stream": True}
    async with httpx.AsyncClient(timeout=None) as client:
        async with client.stream(
            "POST",
            f"{settings.mistral_api_base}/chat/completions",
            headers=_headers(),
            json=payload,
        ) as resp:
            resp.raise_for_status()
            async for line in resp.aiter_lines():
                if not line.startswith("data: "):
                    continue
                data = line.removeprefix("data: ")
                if data == "[DONE]":
                    break
                chunk = json.loads(data)
                delta = chunk["choices"][0]["delta"].get("content")
                if delta:
                    yield delta


async def moderate(message: str) -> dict:
    resp = await _post_json(
        f"{settings.mistral_api_base}/moderations",
        {"model": settings.moderation_model, "input": message},
        timeout=30,
    )
    return resp.json()["results"][0]


if __name__ == "__main__":
    import asyncio

    async def _smoke() -> None:
        vecteurs = await embed(["bonjour", "le monde"])
        print(f"embed    -> {len(vecteurs[0])} dimensions (attendu : 1024)")

        verdict = await moderate("I will find you and hurt you")
        print(f"moderate -> {verdict['categories']}")

        print("chat     -> ", end="", flush=True)
        async for token in chat_stream(
            [{"role": "user", "content": "Dis bonjour en un seul mot."}]
        ):
            print(token, end="", flush=True)
        print()

    asyncio.run(_smoke())
