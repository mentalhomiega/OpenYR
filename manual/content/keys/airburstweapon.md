---
key: AirburstWeapon
summary: The WeaponType supplying the bomblets an Airburst or splitting projectile releases.
see_also: [Splits, Airburst, Cluster, RetargetAccuracy]
when_omitted:
  kind: value
  value: none
---

An [`Airburst=yes`](/keys/airburst/) projectile and a [`Splits=yes`](/keys/splits/) projectile read the weapon. Naming a weapon here changes nothing on any other projectile.

Each bomblet takes its [`Projectile`](/keys/projectile/) and [`Warhead`](/keys/warhead/#scope-weapontype) from the named weapon, A splitting bomblet deals ten times the weapon's [`Damage`](/keys/damage/#scope-weapontype), and an airburst bomblet deals the `Damage` as written. A [`Ranged=yes`](/keys/ranged/) bomblet flies as far as the weapon's [`Range`](/keys/range/#scope-weapontype). None of the carrier's figures reach a bomblet, except that whoever fired the carrier is credited with what the bomblets kill.

The bomblets start from where the carrier detonated, pointing straight down, at the named weapon's [`Speed`](/keys/speed/#scope-weapontype). Only the launch uses that speed: a homing bomblet then works toward a fixed ceiling of 50 leptons per game frame, about `Speed=20`, whatever its weapon sets. A homing bomblet whose weapon's `Speed` is `20` or more therefore slows after launch, at the rate its projectile's [`Acceleration`](/keys/acceleration/#scope-bullettype) sets. [`RetargetAccuracy`](/keys/retargetaccuracy/) decides which target each bomblet is aimed at. From then on each bomblet flies, hits and detonates as its own projectile type dictates.

A name that matches no weapon section registers a new, empty weapon instead of raising an error. `none` and `<none>` leave the setting holding no weapon at all.

:::caution[A projectile with no usable weapon here fires no bomblets]
When an `Airburst=yes` or `Splits=yes` projectile detonates, it releases nothing if this key names no weapon with a `Projectile`. That covers all of these:

- the key is absent, or set to `none` or `<none>`;
- the name is misspelled, which registers an empty weapon with no projectile;
- the named weapon's section sets no `Projectile`.
:::
