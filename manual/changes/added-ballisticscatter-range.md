---
title: Set a projectile's scatter range with BallisticScatter.Min and .Max
category: feature
release: 0.2.0
targets:
- type: key
  id: BallisticScatter.Min
  effect: added
- type: key
  id: BallisticScatter.Max
  effect: added
credit:
- MentalHomiega
---

`BallisticScatter.Min=` and `BallisticScatter.Max=` in a projectile section replace the half and full `[CombatDamage]` `BallisticScatter` bounds on how far an inaccurate arcing shell moves its aim point, in cells.
