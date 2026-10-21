---
title: Burst an Airburst projectile over nine cells
category: fix
release: 0.2.0
targets:
- type: key
  id: Airburst
  effect: changed
- type: key
  id: Splits
  effect: changed
- type: key
  id: AirburstWeapon
  effect: changed
credit: [MentalHomiega]
---

An `Airburst=yes` projectile now fires its `AirburstWeapon` once at the cell it burst over and its eight neighbours, as in Yuri's Revenge, instead of splitting into `Cluster` bomblets. `Splits` now defaults to `no` rather than to `Airburst`, so `Splits=yes` is needed to keep the old splitting.
