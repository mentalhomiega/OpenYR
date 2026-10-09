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
  id: Teleporter
  effect: changed
credit: [MentalHomiega]
---

A teleport locomotor no longer jumps at once. It waits a warp-out delay set by `ChronoTrigger`, `ChronoDistanceFactor`, `ChronoMinimumDelay` and `ChronoRangeMinimum`, then lands and holds for `ChronoDelay` frames. A Chrono Miner's AI waits through both, as it does in Yuri's Revenge. Before, the jump was instant and nothing held the unit after it landed.
