---
title: Stop a jumpjet that is too low, instead of easing its speed
category: fix
release: 0.2.0
targets:
- type: key
  id: Climb
  effect: changed
credit: [MentalHomiega]
---

A jumpjet that is less than half its flight level above the ground or structures under it now stops until it has climbed, unless it is on the cell it is flying to. Before, its speed was only multiplied by 0.9 on each frame it was too low, so it slid to a halt over several frames.
