CREATE TABLE "RoomBan" (
  "id" TEXT NOT NULL,
  "roomId" TEXT NOT NULL,
  "userId" TEXT NOT NULL,
  "createdAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
  CONSTRAINT "RoomBan_pkey" PRIMARY KEY ("id")
);

CREATE UNIQUE INDEX "RoomBan_roomId_userId_key" ON "RoomBan"("roomId", "userId");
CREATE INDEX "RoomBan_roomId_idx" ON "RoomBan"("roomId");
CREATE INDEX "RoomBan_userId_idx" ON "RoomBan"("userId");

ALTER TABLE "RoomBan"
  ADD CONSTRAINT "RoomBan_roomId_fkey"
  FOREIGN KEY ("roomId") REFERENCES "Room"("id") ON DELETE CASCADE ON UPDATE CASCADE;

ALTER TABLE "RoomBan"
  ADD CONSTRAINT "RoomBan_userId_fkey"
  FOREIGN KEY ("userId") REFERENCES "User"("id") ON DELETE CASCADE ON UPDATE CASCADE;

DELETE FROM "RoomPresence" p
WHERE NOT EXISTS (
  SELECT 1 FROM "RoomMember" m
  WHERE m."roomId" = p."roomId" AND m."userId" = p."userId"
);

ALTER TABLE "RoomPresence"
  ADD CONSTRAINT "RoomPresence_roomId_userId_fkey"
  FOREIGN KEY ("roomId", "userId")
  REFERENCES "RoomMember"("roomId", "userId")
  ON DELETE CASCADE ON UPDATE CASCADE;
