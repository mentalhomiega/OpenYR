---
title: Read the second launch point from the art section
category: fix
release: 0.2.0
targets:
- type: key
  id: SecondSpawnOffset
  effect: changed
credit: [MentalHomiega]
---

`SecondSpawnOffset` is now read from the object's Image section in `artmd.ini`, as Yuri's Revenge reads it, and no longer from `rulesmd.ini`. The Boomer's second cruise missile now leaves from the point its art sets. Before, the stock value was never read and every launch left from the same point.
