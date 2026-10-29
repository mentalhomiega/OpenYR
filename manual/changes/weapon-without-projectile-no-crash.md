---
title: Do not crash when a weapon with no projectile is used
category: fix
release: 0.2.0
targets:
- type: key
  id: Projectile
  effect: changed
credit:
- MentalHomiega
---

A weapon with no section, with no [`Projectile=`](/keys/projectile/), or with `Projectile=none` no longer crashes the game when a unit, structure or trigger uses it. The weapon cannot fire. The debug log gets one `has no projectile` line for it each time the game starts. A weapon whose section names a bullet type is unchanged, including one whose bullet type has no section.
