---
title: Launch a nuclear missile at the speed Yuri's Revenge uses
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A missile silo now starts its missile at 10 leptons a frame, lets it speed up to 255, and gives it the `Damage=` of the launching weapon. Before, the missile started at 100, was limited by the weapon's `Speed=`, and always had a strength of 200. A missile climbs to its detonation height about 15 frames later than before.
