---
title: Hold a chronosphere-moved unit on its cell for ChronoDelay before it lands
category: fix
release: 0.2.0
targets:
- type: key
  id: ChronoDelay
  effect: changed
- type: key
  id: WarpOut
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A unit that a chronosphere picks stays on its cell under `WarpOut=` for `ChronoDelay` frames, then lands at the destination with `WarpIn=`. Before, it landed at once while the warp-out animation was still playing. While it waits, it is drawn half transparent and takes no unforced damage. A Chrono Legionnaire or Chrono Miner keeps the distance-based timing of the teleport locomotor.
