---
title: Fade the prism support beam over its duration
category: fix
release: 0.2.0
targets:
- type: system
  id: prism-towers
  effect: changed
credit:
- MentalHomiega
---

A prism support beam now fades from full intensity to none over `PrismSupportDuration` frames, as gamemd's `BuildingClass::FireLaser` (0x44ABD0) sets it. Before, it stayed at full intensity for its whole life.
