export const RESULT_DISPLAY_MS = 5_000;

export type Answer = 'FACT' | 'FAKE';

export function calculatePayout(totalStake: number, goodStake: number, playerStake: number, playerAnsweredCorrectly: boolean): number {
  if (!playerAnsweredCorrectly || totalStake <= 0 || goodStake <= 0 || playerStake <= 0) return 0;
  return Math.floor((2 - goodStake / totalStake) * playerStake * 1.1);
}

export function effectiveMinimumStake(balance: number): number {
  if (!Number.isSafeInteger(balance) || balance <= 0) return 0;
  return balance >= 10 ? 10 : balance;
}
