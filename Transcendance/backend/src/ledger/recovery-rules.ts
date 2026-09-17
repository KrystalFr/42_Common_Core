export type RecoveryState = {
  virtualBalance: number;
  bankruptEpisode: number;
  recoveredEpisode: number;
  recoveryEligibleEpisode: number | null;
};

export function isRecoveryAvailable(user: RecoveryState): boolean {
  return (
    user.virtualBalance === 0 &&
    user.bankruptEpisode > user.recoveredEpisode &&
    user.recoveryEligibleEpisode === user.bankruptEpisode
  );
}
