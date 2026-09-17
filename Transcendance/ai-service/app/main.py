import logging

from fastapi import FastAPI, HTTPException

from app.config import settings
from app.db import connect, ensure_schema
from app.rag.charger_noyau import charger_si_vide
from app.rag.router import router as rag_router
from app.llm.router import router as llm_router
from app.moderation.router import router as moderation_router

if not settings.llm_api_key:
    logging.getLogger("uvicorn.error").warning(
        "LLM_API_KEY est vide : les appels Mistral échoueront. Renseigne la clé dans .env."
    )

app = FastAPI(title="FactArena AI Service")


@app.on_event("startup")
def preparer_base() -> None:
    try:
        ensure_schema()
        inseres = charger_si_vide()
        if inseres:
            logging.getLogger("uvicorn.error").info(
                "Index vectoriel initialisé avec %d passages livrés.", inseres
            )
    except Exception as erreur:
        logging.getLogger("uvicorn.error").warning(
            "Schéma vectoriel non créé au démarrage (%s). Il le sera à la première ingestion.",
            erreur,
        )

app.include_router(rag_router, prefix="/rag", tags=["rag"])

app.include_router(llm_router, prefix="/llm", tags=["llm"])

app.include_router(moderation_router, prefix="/moderation", tags=["moderation"])


@app.get("/health")
def health():
    return {"status": "ok"}


@app.get("/ready")
def ready():
    try:
        with connect() as conn, conn.cursor() as cur:
            cur.execute("SELECT to_regclass('rag.documents');")
            corpus = 0
            if cur.fetchone()[0] is not None:
                cur.execute("SELECT count(*) FROM documents;")
                corpus = cur.fetchone()[0]
    except Exception as exc:
        raise HTTPException(status_code=503, detail=f"Base de données injoignable : {exc}")
    return {
        "status": "ready",
        "corpus_documents": corpus,
        "llm_key_configuree": bool(settings.llm_api_key),
    }
