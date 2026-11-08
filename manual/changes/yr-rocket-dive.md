---
title: Start Dreadnought and Boomer missile dives farther out
category: fix
release: 0.2.0
targets:
- type: system
  id: spawned-aircraft
  effect: changed
credit: [MentalHomiega]
---

A Dreadnought or Boomer missile with `LazyCurve=no` now starts its dive once its distance to the target across the ground is no more than its height above the target. It comes down over the last few cells, where it used to stay level over the target and fall straight down. V3 rockets, which use `LazyCurve=yes`, fly as before. Multiplayer games with an earlier build diverge once one of these missiles dives.
