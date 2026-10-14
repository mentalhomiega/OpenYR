---
title: Color house-colored laser beams from the house laser color
category: fix
release: 0.2.0
targets:
- type: key
  id: IsHouseColor
  effect: changed
- type: key
  id: DiskLaser
  effect: changed
- type: system
  id: prism-towers
  effect: changed
credit:
- MentalHomiega
---

A house-colored laser beam now takes its core from the house laser color, a brightness-normalized form of the house color, as gamemd does. Before, the core used the house color as it is. Its glow still is half the core. The glow's spread now comes from `LaserOuterSpread`, where before it was zero. The prism support beam and the disk laser use the same color.
