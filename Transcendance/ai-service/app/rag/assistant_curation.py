import asyncio
import re
import sys
from urllib.parse import unquote

import httpx

from app.config import settings
from app.mistral_client import chat_stream
from app.rag.search import search_similar

ENTETES = {"User-Agent": "FactArena-42-project/0.1 (ft_transcendence; educational)"}
MODELE_AUDIO = "voxtral-small-latest"
MODELE_VISION = "mistral-small-latest"   # multimodal : lit les miniatures YouTube
TAILLE_MAX_AUDIO = 20 * 1024 * 1024


def _titre(client: httpx.Client, url: str) -> str:
    if "youtu" in url:
        try:
            r = client.get(
                "https://www.youtube.com/oembed",
                params={"url": url, "format": "json"}, timeout=20,
            )
            return r.json().get("title", "") if r.status_code == 200 else ""
        except Exception:
            return ""
    if "upload.wikimedia.org" in url:
        nom = unquote(url.split("/")[-1].split("?", 1)[0])
        return re.sub(r"\.(webm|ogv|mp4|ogg)$", "", nom, flags=re.I).replace("_", " ")
    return ""


def _decrire_miniature(client: httpx.Client, url: str) -> str:
    m = re.search(r"(?:/embed/|v=|youtu\.be/)([\w-]{6,})", url)
    if not m:
        return ""
    vignette = f"https://img.youtube.com/vi/{m.group(1)}/maxresdefault.jpg"
    try:
        r = client.post(
            f"{settings.mistral_api_base}/chat/completions",
            headers={"Authorization": f"Bearer {settings.llm_api_key}",
                     "Content-Type": "application/json"},
            json={"model": MODELE_VISION, "messages": [{"role": "user", "content": [
                {"type": "text", "text": "Décris en deux phrases ce que montre cette image, "
                                         "sans interpréter ni juger."},
                {"type": "image_url", "image_url": vignette}]}]},
            timeout=90,
        )
        if r.status_code != 200:
            return ""
        return r.json()["choices"][0]["message"]["content"].strip()
    except Exception:
        return ""


def _transcrire(client: httpx.Client, url: str) -> str:
    if not re.search(r"\.(webm|ogv|mp4|ogg|mp3|wav)(\?|$)", url, re.I):
        return ""
    try:
        media = client.get(url, headers=ENTETES, timeout=90, follow_redirects=True)
        if media.status_code != 200 or len(media.content) > TAILLE_MAX_AUDIO:
            return ""
        r = client.post(
            f"{settings.mistral_api_base}/audio/transcriptions",
            headers={"Authorization": f"Bearer {settings.llm_api_key}"},
            files={"file": ("extrait.webm", media.content)},
            data={"model": MODELE_AUDIO},
            timeout=180,
        )
        return r.json().get("text", "").strip() if r.status_code == 200 else ""
    except Exception:
        return ""


CONSIGNE = (
    "Tu prépares une question pour un jeu de fact-checking. Le joueur lit une "
    "affirmation et doit deviner si elle est vraie ou fausse.\n\n"
    "Règles impératives :\n"
    "- Rédige l'affirmation du point de vue de la RUMEUR qui circule, jamais de "
    "celui du vérificateur. N'écris jamais « prouve que », « démontre que », "
    "« contrairement à ».\n"
    "- N'emploie aucun mot qui révèle la réponse : ni faux, truqué, manipulé, "
    "généré par IA, ni véritable, authentique, réel.\n"
    "- Une phrase affirmative, jamais une question, entre 25 et 300 caractères.\n"
    "- Ne DÉCIDE pas du verdict : propose-le, et dis franchement « INCERTAIN » "
    "si les sources fournies ne permettent pas de trancher.\n"
    "- IMPÉRATIF : l'affirmation doit décrire CE QUE MONTRE CETTE VIDÉO-CI. Les "
    "fact-checks fournis ne sont que des pistes : s'ils portent sur un événement "
    "VOISIN mais différent, ignore-les et réponds INCERTAIN. Rattacher une vidéo "
    "authentique au démenti d'une autre affaire fabriquerait une fausse réponse.\n\n"
    "Réponds exactement dans ce format :\n"
    "AFFIRMATION: <la phrase>\n"
    "VERDICT_PROPOSE: VRAI | FAUX | INCERTAIN\n"
    "JUSTIFICATION: <une phrase, en citant la source si elle existe>\n"
    "SOURCE: <url de la source utilisée, ou AUCUNE>"
)


async def main(url: str) -> None:
    with httpx.Client() as client:
        print("→ métadonnées...")
        titre = _titre(client, url)
        print(f"   titre : {titre or '(inconnu)'}")

        print("→ transcription de la bande-son...")
        transcription = _transcrire(client, url)
        if transcription:
            print(f"   {transcription[:160]}...")
        else:
            print("   (indisponible : fichier non direct ou son absent)")

        description = ""
        if not transcription:
            print("→ analyse de la miniature...")
            description = _decrire_miniature(client, url)
            print(f"   {description[:160] or '(indisponible)'}")

    requete = " ".join(filter(None, [titre, transcription[:400], description[:200]]))
    if not requete.strip():
        print("\nImpossible d'analyser : ni titre ni transcription.")
        return

    print("→ recherche d'un fact-check dans le corpus...")
    try:
        passages = await search_similar(requete, k=4)
    except Exception:
        passages = []
    for p in passages[:3]:
        print(f"   • {p['content'][:100]}")

    contexte = "\n\n".join(f"[{i + 1}] {p['content']}\n(source: {p['url']})"
                           for i, p in enumerate(passages))
    messages = [
        {"role": "system", "content": CONSIGNE},
        {"role": "user", "content": (
            f"Titre de la vidéo : {titre or '(inconnu)'}\n"
            f"Transcription : {transcription[:1500] or '(indisponible)'}\n"
            f"Ce que montre l'image : {description or '(indisponible)'}\n\n"
            f"Fact-checks trouvés dans le corpus :\n{contexte or '(aucun)'}"
        )},
    ]

    print("→ rédaction de la fiche...\n")
    morceaux = [m async for m in chat_stream(messages)]
    fiche = "".join(morceaux).strip()

    sans_source = re.search(r"^SOURCE:\s*(AUCUNE|$)", fiche, re.M | re.I)
    verdict_tranche = re.search(r"^VERDICT_PROPOSE:\s*(VRAI|FAUX)", fiche, re.M | re.I)
    if sans_source and verdict_tranche:
        fiche = re.sub(
            r"^VERDICT_PROPOSE:.*$",
            "VERDICT_PROPOSE: INCERTAIN  (forcé : aucune source ne l'étaye)",
            fiche, flags=re.M,
        )

    print("=" * 66)
    print(fiche)
    print("=" * 66)
    print(f"\nVIDÉO : {url}")
    print("\n⚠️  PROPOSITION à valider par un humain. Si le verdict est INCERTAIN,")
    print("   ne crée pas la claim : mieux vaut moins de contenu que du contenu faux.")


if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage : python -m app.rag.assistant_curation <url de la vidéo>")
        raise SystemExit(1)
    asyncio.run(main(sys.argv[1]))
