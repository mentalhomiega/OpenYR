---
title: Read launch-file bool keys without regard to case
category: fix
release: 0.2.0
targets:
- type: format
  id: spawn-ini
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

The launch file's bool keys match without regard to case, as the game's own reader does. A client that writes `Superweapons=False` now switches off, for the whole match, the superweapons the shell can disable. Before, the spawner looked for the key as `SuperWeapons` in that exact case and left the superweapons on. A client match that should have superweapons needs a check before it is relied on. The launch-file format version is now 8, and the spawner includes it in the match identity, so a build that still reads the file the old way does not describe the same match.
