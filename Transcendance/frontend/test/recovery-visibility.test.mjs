import test from 'node:test';
import assert from 'node:assert/strict';
import fs from 'node:fs';

const room = fs.readFileSync(new URL('../src/pages/Room.tsx', import.meta.url), 'utf8');

test('recovery button remains visible during the next in-progress round', () => {
  assert.match(room, /const recoveryVisible = recoveryAvailable;/);
  assert.doesNotMatch(
    room,
    /const recoveryVisible = recoveryAvailable && \(!game \|\| round\?\.status === 'REVEALED' \|\| round\?\.status === 'FINISHED'\);/
  );
});
