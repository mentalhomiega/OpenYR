---
title: Leave a type that a rule key names late at its defaults
category: fix
release: 0.2.0
targets:
- type: key
  id: DeathWeapon
  scope: global-rules
  effect: changed
- type: key
  id: AirburstWeapon
  effect: changed
- type: key
  id: NukeProjectile
  effect: changed
- type: key
  id: ReadLateTypes
  effect: added
credit:
- MentalHomiega
---

A weapon or projectile that first appears in a key read after its type list, such as [`DeathWeapon=`](/keys/deathweapon/#scope-global-rules) in `[CombatDamage]`, `NukeProjectile=` in `[SpecialWeapons]`, or a projectile's [`AirburstWeapon=`](/keys/airburstweapon/), keeps its defaults, as in gamemd. [`ReadLateTypes=yes`](/keys/readlatetypes/#scope-global-rules) reads the sections of such types. Warheads that `[CombatDamage]` and `[SpecialWeapons]` name are still read, and stock rules are unaffected.
