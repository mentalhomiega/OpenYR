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

The launch file's bool keys match without regard to case, as the game's own reader does. A client that writes `Superweapons=False` now withholds, for the whole match, the superweapons whose type has `DisableableFromShell=yes`. The CnCNet client always writes it and controls the choice through each superweapon's `DisableableFromShell` in the map, so its Super weapons option now works. Before, the spawner looked for the key as `SuperWeapons` in that exact case and ignored it. The launch-file format version is now 8, and the spawner includes it in the match identity, so a build that still reads the file the old way does not describe the same match.
