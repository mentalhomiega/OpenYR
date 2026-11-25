---
title: Read the section of a type that only a rule key names
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

A weapon, projectile, warhead, animation or other type that appears first in a rules key read late, such as [`DeathWeapon=`](/keys/deathweapon/#scope-global-rules) in `[CombatDamage]`, `NukeProjectile=` in `[SpecialWeapons]` or a projectile's [`AirburstWeapon=`](/keys/airburstweapon/), now reads its own section. Before, it kept its defaults, so a `DeathWeapon` with `Projectile=` and `Warhead=` in its section was given the default projectile and warhead instead. gamemd never reads such a section; [`ReadLateTypes=no`](/keys/readlatetypes/#scope-global-rules) selects that single pass. Stock rules are unaffected.
