---
title: Teleport jumps at once and holds for a distance-based delay
category: fix
release: 0.2.0
targets:
- type: key
  id: ChronoDelay
  effect: changed
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
  id: WarpOut
  effect: changed
- type: key
  id: Teleporter
  effect: changed
credit: [MentalHomiega]
---

A teleport locomotor jumps when its order is processed. The object leaves its cell and appears at the destination in the same frame, as gamemd does (0x7192F0 and 0x719715). It then holds still for the larger of `ChronoMinimumDelay` and its distance divided by `ChronoDistanceFactor`, or `ChronoMinimumDelay` alone under `ChronoRangeMinimum` and when `ChronoTrigger` is `no`. `ChronoDelay` is no longer read by the teleport locomotor. An order given during the hold is refused. A Chrono Miner holds the same way on its refinery trip. A landing on a cell the object cannot stand on moves to the nearest cell it can.
