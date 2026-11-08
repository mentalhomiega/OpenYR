---
title: Show every passenger pip and a gunner's own pip
category: fix
release: 0.2.0
targets:
- type: key
  id: PipScale
  effect: changed
- type: key
  id: MaxPips
  effect: changed
- type: key
  id: Gunner
  effect: changed
credit:
- MentalHomiega
---

A transport with `PipScale=Passengers` now shows one pip for each of its `Passengers`, as in Yuri's Revenge, where it showed at most five. A passenger takes pips for its `Size`: a soldier's first pip has its `Pip` color, a vehicle's is blue and the rest are white. A `Gunner=yes` type shows its first pip apart from the others.
