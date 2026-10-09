---
title: Delay a teleport's jump and the hold after it
category: fix
release: 0.2.0
targets:
- type: key
  id: ChronoDelay
  effect: added
- type: key
  id: ChronoTrigger
  effect: added
- type: key
  id: ChronoDistanceFactor
  effect: added
- type: key
  id: ChronoMinimumDelay
  effect: added
- type: key
  id: ChronoRangeMinimum
  effect: added
- type: key
  id: WarpIn
  effect: added
- type: key
  id: WarpOut
  effect: changed
- type: key
  id: Teleporter
  effect: changed
credit: [MentalHomiega]
---

A teleport locomotor no longer jumps at once. It waits a warp-out delay set by `ChronoTrigger`, `ChronoDistanceFactor`, `ChronoMinimumDelay` and `ChronoRangeMinimum`, then lands and holds for `ChronoDelay` frames. A Chrono Miner has no warp-out delay, and it still holds after landing. The warp plays `WarpOut` where the object stands when ordered, and `WarpIn` where it lands. A landing outside the local map area, or on a cell the object cannot stand on, moves to the nearest cell it can. Before, the jump was instant and nothing held the unit after it landed.
