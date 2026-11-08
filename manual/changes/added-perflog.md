---
title: Log per-second timing from RA2MD.INI
category: internal
release: 0.2.0
targets:
- type: key
  id: PerfLog
  effect: added
credit: [MentalHomiega]
---

`PerfLog=yes` in the `[Options]` section of `RA2MD.INI` writes timing for the game frame, the draw and the present to the debug log once a second, with counts of objects, voxels and texture uploads. It is off by default. The setting does not change the game.
