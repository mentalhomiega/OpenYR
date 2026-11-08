---
title: Set off the current turret's weapon when a turreted object explodes
category: fix
release: 0.2.0
targets:
- type: key
  id: DeathWeapon
  effect: changed
credit: [MentalHomiega]
---

An object with `TurretCount=` and no `DeathWeapon=` now sets off the weapon at its current turret position when it sets off a death weapon, as Yuri's Revenge does. That happens when it dies with `Explodes=yes`, or with a current weapon that is `Suicide=yes`. Before, it used its first weapon. Objects without turrets still use their first weapon.
