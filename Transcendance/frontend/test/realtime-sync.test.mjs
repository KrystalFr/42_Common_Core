import test from 'node:test';
import assert from 'node:assert/strict';
import { readFile } from 'node:fs/promises';

const profile = await readFile(new URL('../src/pages/Profile.tsx', import.meta.url), 'utf8');
const auth = await readFile(new URL('../src/auth/AuthContext.tsx', import.meta.url), 'utf8');
const friends = await readFile(new URL('../src/pages/Friends.tsx', import.meta.url), 'utf8');

test('Profile uses canonical invalidations and abortable HTTP requests', () => {
  assert.match(profile, /factarena:stats-invalidate/);
  assert.match(profile, /factarena:history-invalidate/);
  assert.match(profile, /new AbortController\(\)/);
  assert.match(profile, /signal: controller\.signal/);
  assert.doesNotMatch(profile, /\.catch\(\(\) => setStats\(null\)\)/);
});

test('Profile cleans every Socket-relay listener it installs', () => {
  const pairs = [
    ['factarena:stats-invalidate', 'removeEventListener\(\'factarena:stats-invalidate\''],
    ['factarena:history-invalidate', 'removeEventListener\(\'factarena:history-invalidate\''],
    ['factarena:balance-update', 'removeEventListener\(\'factarena:balance-update\''],
    ['factarena:profile-update', 'removeEventListener\(\'factarena:profile-update\''],
  ];
  for (const [event, cleanup] of pairs) {
    assert.ok(profile.includes(event));
    assert.ok(profile.includes(cleanup));
  }
});

test('AuthContext exposes only the canonical realtime protocols', () => {
  assert.match(auth, /socket\.on\('stats:invalidate'/);
  assert.match(auth, /socket\.on\('leaderboard:invalidate'/);
  assert.match(auth, /socket\.on\('history:invalidate'/);
  assert.match(auth, /socket\.on\('friend:request-received'/);
  assert.match(auth, /socket\.on\('friend:request-accepted'/);
  assert.match(auth, /socket\.on\('friend:request-rejected'/);
  assert.match(auth, /socket\.on\('friend:removed'/);
  assert.doesNotMatch(auth, /socket\.on\('leaderboard:update'/);
  assert.doesNotMatch(auth, /socket\.on\('history:update'/);
  assert.doesNotMatch(auth, /socket\.on\('friend:update'/);
});

test('Friends applies presence only to the matching friend', () => {
  assert.match(friends, /ami\.id === presence\.userId/);
  assert.match(friends, /factarena:presence-update/);
});
