---
title: Let a warhead stop the death weapons of what it kills (DeathWeapon.Suppress)
category: feature
release: 0.2.0
targets:
- type: key
  id: DeathWeapon.Suppress
  effect: added
- type: key
  id: DeathWeapon.SuppressInfantry
  effect: added
- type: key
  id: DeathWeapon.SuppressVehicles
  effect: added
credit: [MentalHomiega]
---

A warhead with `DeathWeapon.Suppress`, `DeathWeapon.SuppressInfantry=yes` or `DeathWeapon.SuppressVehicles=yes` now kills the matching objects without their death weapon going off, as Ares does. Warheads that leave the keys out are unchanged.
