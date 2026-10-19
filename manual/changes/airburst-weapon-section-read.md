---
title: Read the section of a weapon that only an AirburstWeapon names
category: fix
release: 0.2.0
targets:
- type: key
  id: AirburstWeapon
  effect: changed
credit:
- MentalHomiega
---

A weapon that no `[Weapons]` entry or other type names and that only a projectile's `AirburstWeapon=` names used to keep the built-in defaults, including having no projectile, unless it was registered. It now reads its section from the same rules file as the projectile, and a projectile that weapon names is read too. A projectile whose airburst weapon has no projectile no longer ends the game with a crash.
