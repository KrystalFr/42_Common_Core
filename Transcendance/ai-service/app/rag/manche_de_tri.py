import argparse
import secrets
import string
from datetime import datetime, timedelta, timezone

from app.db import connect

DUREE_SECONDES = 3600


def _cuid() -> str:
    alphabet = string.ascii_lowercase + string.digits
    return "c" + "".join(secrets.choice(alphabet) for _ in range(24))


def main() -> None:
    parseur = argparse.ArgumentParser(description="Manche de curation")
    parseur.add_argument("--email", default="admin@factarena.local",
                         help="propriétaire du salon de tri")
    options = parseur.parse_args()

    with connect() as conn, conn.cursor() as cur:
        cur.execute('SELECT id FROM "User" WHERE email = %s', (options.email,))
        utilisateur = cur.fetchone()
        if not utilisateur:
            print(f"Compte introuvable : {options.email}")
            return
        user_id = utilisateur[0]

        cur.execute("""SELECT id FROM "Claim"
                       WHERE "mediaRef" LIKE '%upload.wikimedia%'
                          OR "mediaRef" LIKE '%/embed/%'
                          OR "mediaRef" LIKE '%plugins/video.php%'
                       ORDER BY category, text""")
        claims = [r[0] for r in cur.fetchall()]
        if not claims:
            print("Aucun clip jouable.")
            return

        maintenant = datetime.now(timezone.utc)
        room_id, round_id = _cuid(), _cuid()
        cur.execute(
            'INSERT INTO "Room" (id, name, status, "createdBy", "createdAt", "updatedAt") '
            'VALUES (%s, %s, %s::"RoomStatus", %s, %s, %s)',
            (room_id, f"Tri du corpus — {maintenant:%d/%m %H:%M}", "IN_PROGRESS",
             user_id, maintenant, maintenant),
        )
        ouvre = maintenant + timedelta(seconds=3)
        cur.execute(
            'INSERT INTO "Round" (id, "roomId", status, "opensAt", "locksAt", '
            '"createdAt", "updatedAt") '
            'VALUES (%s, %s, %s::"RoundStatus", %s, %s, %s, %s)',
            (round_id, room_id, "COUNTDOWN", ouvre,
             ouvre + timedelta(seconds=DUREE_SECONDES), maintenant, maintenant),
        )
        cur.executemany(
            'INSERT INTO "RoundClaim" (id, "roundId", "claimId", "orderIndex") '
            'VALUES (%s, %s, %s, %s)',
            [(_cuid(), round_id, claim_id, i) for i, claim_id in enumerate(claims)],
        )
        conn.commit()

    minutes = DUREE_SECONDES // 60
    print(f"Manche de tri créée : {len(claims)} clips, {minutes} minutes.\n")
    print(f"  https://localhost:8443/room/{room_id}\n")
    print("Parcours les clips, clique « cette vidéo ne correspond pas à")
    print("l'affirmation » sur ceux qui clochent, puis :")
    print("  docker compose exec ai-service python -m app.rag.signalements --ecarter")


if __name__ == "__main__":
    main()
