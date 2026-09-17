DEFAULT_CHUNK_CHARS = 800


def chunk_text(text: str, size: int = DEFAULT_CHUNK_CHARS) -> list[str]:
    chunks: list[str] = []
    current = ""
    for para in text.split("\n"):
        para = para.strip()
        if not para:
            continue
        if current and len(current) + len(para) + 1 > size:
            chunks.append(current)
            current = para
        else:
            current = f"{current}\n{para}" if current else para
    if current:
        chunks.append(current)
    return chunks
