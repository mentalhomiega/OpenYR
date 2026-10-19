---
title: Read MigAttackCursor and the FloatBeach land figure
category: feature
release: 0.2.0
targets:
- type: key
  id: MigAttackCursor
  effect: added
- type: enum
  id: SpeedType
  effect: changed
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

`MigAttackCursor=yes` on a weapon shows the airstrike cursor over a structure that allows C4. The terrain table reads `FloatBeach=` where it read `Creep=`, and it no longer reads `Winged=`. A figure that a present land section leaves out is now `1`; before, it kept its earlier value, which was zero on a first read.
