ALTER TABLE "User"
  ADD COLUMN "recoveryEligibleEpisode" INTEGER,
  ADD COLUMN "activeRoomId" TEXT;

CREATE INDEX "User_activeRoomId_idx" ON "User"("activeRoomId");

ALTER TABLE "User"
  ADD CONSTRAINT "User_activeRoomId_fkey"
  FOREIGN KEY ("activeRoomId") REFERENCES "Room"("id")
  ON DELETE SET NULL ON UPDATE CASCADE;

WITH ranked AS (
  SELECT
    rm."id",
    rm."userId",
    rm."roomId",
    ROW_NUMBER() OVER (
      PARTITION BY rm."userId"
      ORDER BY rm."joinedAt" DESC, rm."id" DESC
    ) AS rn
  FROM "RoomMember" rm
  JOIN "Room" r ON r."id" = rm."roomId"
  WHERE r."status" IN ('WAITING', 'IN_PROGRESS')
)
UPDATE "RoomMember" rm
SET "state" = 'FINISHED'
FROM ranked
WHERE rm."id" = ranked."id"
  AND ranked.rn > 1;

WITH current_membership AS (
  SELECT DISTINCT ON (rm."userId")
    rm."userId",
    rm."roomId"
  FROM "RoomMember" rm
  JOIN "Room" r ON r."id" = rm."roomId"
  WHERE r."status" IN ('WAITING', 'IN_PROGRESS')
  ORDER BY rm."userId", rm."joinedAt" DESC, rm."id" DESC
)
UPDATE "User" u
SET "activeRoomId" = current_membership."roomId"
FROM current_membership
WHERE u."id" = current_membership."userId";

