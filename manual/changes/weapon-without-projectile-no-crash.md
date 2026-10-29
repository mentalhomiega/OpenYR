---
title: Fire with the default projectile and warhead when a weapon has none
category: fix
release: 0.2.0
targets:
- type: key
  id: Projectile
  effect: changed
- type: key
  id: Warhead
  effect: changed
- type: key
  id: DeathWeapon
  scope: global-rules
  effect: changed
credit:
- MentalHomiega
---

A weapon with no [`Projectile=`](/keys/projectile/), or with `Projectile=none`, now fires with the default projectile and warhead rather than crashing the game. gamemd crashes on such a weapon. A weapon with a projectile and no [`Warhead=`](/keys/warhead/) fires with the default warhead; before, an infantry with such a weapon crashed the game. The weapon's `Damage=` and `Range=` still apply. The debug log gets one line for each such weapon each time the game starts.

When the last rules file read sets [`DeathWeapon=`](/keys/deathweapon/#scope-global-rules) to a weapon that no rules file declared before, that weapon gets the same default, and an `Explodes=yes` unit with no weapon of its own sets it off when it dies. Before, that death fired nothing.
