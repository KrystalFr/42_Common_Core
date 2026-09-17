import { calculatePayout, effectiveMinimumStake } from '../src/realtime/game-rules.js';

describe('new FACT/FAKE game rules', () => {
  it('uses the exact payout formula and floor', () => {
    expect(calculatePayout(1000, 400, 100, true)).toBe(176);
    expect(calculatePayout(100, 100, 100, true)).toBe(110);
    expect(calculatePayout(1000, 0, 100, true)).toBe(0);
    expect(calculatePayout(0, 0, 100, true)).toBe(0);
    expect(calculatePayout(1000, 400, 100, false)).toBe(0);
  });

  it('allows the whole balance below the normal minimum', () => {
    expect(effectiveMinimumStake(100)).toBe(10);
    expect(effectiveMinimumStake(10)).toBe(10);
    expect(effectiveMinimumStake(5)).toBe(5);
    expect(effectiveMinimumStake(0)).toBe(0);
  });
});
