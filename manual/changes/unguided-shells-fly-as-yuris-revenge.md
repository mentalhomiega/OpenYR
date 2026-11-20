---
title: Fly unguided shells as Yuri's Revenge does
category: fix
release: 0.2.0
targets:
- type: system
  id: projectile-flight
  effect: changed
- type: key
  id: Bouncy
  effect: changed
- type: key
  id: Elasticity
  effect: changed
credit:
- MentalHomiega
---

A projectile with no `ROT` now leaves at the speed for the distance to its target, bursts on its target when it enters the target's cell low, and flies on below the ground while its firer is known, as in Yuri's Revenge. Tank and artillery shells fired up or down slopes, off cliffs and at bridge decks hit far more often. A shell fired from three cells below at a target on the edge of a four-level cliff top goes off on the cliff edge and misses, as it does in Yuri's Revenge. [`Bouncy`](/keys/bouncy/) and [`Elasticity`](/keys/elasticity/#scope-bullettype) are still read but change nothing.
