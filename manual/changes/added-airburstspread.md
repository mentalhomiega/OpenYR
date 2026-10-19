---
title: Fire an airburst weapon at every cell in a spread
category: feature
release: 0.2.0
targets:
- type: key
  id: AirburstSpread
  effect: added
- type: key
  id: AroundTarget
  effect: added
credit:
- MentalHomiega
---

An `Airburst=yes` projectile with `AirburstSpread=` fires its `AirburstWeapon` once at each cell within that many cells of its burst, instead of splitting into `Cluster` bomblets. `AroundTarget=no` centres a splitting or spreading projectile's bomblets on the cell it burst over rather than its target.
