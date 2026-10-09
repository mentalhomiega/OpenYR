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

We made a teleport wait before it jumps. The wait is set by [`ChronoTrigger`](/keys/chronotrigger/), [`ChronoDistanceFactor`](/keys/chronodistancefactor/), [`ChronoMinimumDelay`](/keys/chronominimumdelay/) and [`ChronoRangeMinimum`](/keys/chronorangeminimum/). The object then lands and holds still for [`ChronoDelay`](/keys/chronodelay/) frames. Its AI waits through both, as in Yuri's Revenge. Before, the jump was instant and nothing held the unit after it landed.
