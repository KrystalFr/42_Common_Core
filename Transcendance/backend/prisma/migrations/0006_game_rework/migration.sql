ALTER TYPE "TransactionType" ADD VALUE IF NOT EXISTS 'PENALTY';
CREATE TYPE "GameStatus" AS ENUM ('STARTING', 'IN_PROGRESS', 'FINISHED');
CREATE TYPE "BetAnswer" AS ENUM ('FACT', 'FAKE');

ALTER TABLE "User"
  ADD COLUMN "bankruptEpisode" INTEGER NOT NULL DEFAULT 0,
  ADD COLUMN "recoveredEpisode" INTEGER NOT NULL DEFAULT 0;

ALTER TABLE "Room" ADD COLUMN "lobbyExpiresAt" TIMESTAMP(3);

CREATE TABLE "RoomMember" (
  "id" TEXT NOT NULL,
  "roomId" TEXT NOT NULL,
  "userId" TEXT NOT NULL,
  "ready" BOOLEAN NOT NULL DEFAULT false,
  "joinedAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
  "updatedAt" TIMESTAMP(3) NOT NULL,
  "state" TEXT NOT NULL DEFAULT 'NOT_READY',
  CONSTRAINT "RoomMember_pkey" PRIMARY KEY ("id")
);
CREATE UNIQUE INDEX "RoomMember_roomId_userId_key" ON "RoomMember"("roomId", "userId");
CREATE INDEX "RoomMember_roomId_ready_idx" ON "RoomMember"("roomId", "ready");
ALTER TABLE "RoomMember" ADD CONSTRAINT "RoomMember_roomId_fkey" FOREIGN KEY ("roomId") REFERENCES "Room"("id") ON DELETE CASCADE ON UPDATE CASCADE;
ALTER TABLE "RoomMember" ADD CONSTRAINT "RoomMember_userId_fkey" FOREIGN KEY ("userId") REFERENCES "User"("id") ON DELETE CASCADE ON UPDATE CASCADE;

CREATE TABLE "Game" (
  "id" TEXT NOT NULL,
  "roomId" TEXT NOT NULL,
  "status" "GameStatus" NOT NULL DEFAULT 'STARTING',
  "startedAt" TIMESTAMP(3) NOT NULL DEFAULT CURRENT_TIMESTAMP,
  "finishedAt" TIMESTAMP(3),
  "currentRoundIndex" INTEGER NOT NULL DEFAULT 0,
  CONSTRAINT "Game_pkey" PRIMARY KEY ("id")
);
CREATE UNIQUE INDEX "Game_roomId_key" ON "Game"("roomId");
CREATE INDEX "Game_status_idx" ON "Game"("status");
ALTER TABLE "Game" ADD CONSTRAINT "Game_roomId_fkey" FOREIGN KEY ("roomId") REFERENCES "Room"("id") ON DELETE CASCADE ON UPDATE CASCADE;

ALTER TABLE "Round" ADD COLUMN "gameId" TEXT;
CREATE INDEX "Round_gameId_idx" ON "Round"("gameId");
ALTER TABLE "Round" ADD CONSTRAINT "Round_gameId_fkey" FOREIGN KEY ("gameId") REFERENCES "Game"("id") ON DELETE CASCADE ON UPDATE CASCADE;

ALTER TABLE "Bet"
  ADD COLUMN "answer" "BetAnswer",
  ADD COLUMN "amountConfirmedAt" TIMESTAMP(3),
  ADD COLUMN "answeredAt" TIMESTAMP(3),
  ADD COLUMN "debitedAt" TIMESTAMP(3);
