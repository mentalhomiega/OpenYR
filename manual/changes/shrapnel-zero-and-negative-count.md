---
title: Fire shrapnel at every enemy for a zero count and by distance for a negative one
category: fix
release: 0.2.0
targets:
- type: key
  id: ShrapnelCount
  effect: changed
credit: [MentalHomiega]
---

A `ShrapnelCount` of `0` now sends a piece at every enemy in range, with no limit, as in Yuri's Revenge. A negative count now gives its absolute value minus the firer's distance from the impact in whole cells, or three pieces when there is no firer. Before, a count of `0` or below threw nothing.
