---
key: Splits
summary: Breaks the projectile into a burst of bomblets when it detonates.
see_also: [Airburst, AirburstWeapon, Cluster, RetargetAccuracy]
when_omitted:
  kind: value
  value: "no"
---

A splitting projectile detonates once and then releases [`Cluster`](/keys/cluster/) bomblets of its [`AirburstWeapon`](/keys/airburstweapon/). An ordinary projectile instead detonates `Cluster` times around the point of impact. The `AirburstWeapon` page describes what each bomblet takes from that weapon, and [`RetargetAccuracy`](/keys/retargetaccuracy/) decides what each one is aimed at.

An [`Airburst=yes`](/keys/airburst/) projectile does not split unless it writes `Splits=yes`. It fires its weapon at the nine cells around where it burst.

The splitting projectile's blast is never pulled onto a nearby target. That is the third move in [Where the blast lands](/systems/projectile-flight/#where-the-blast-lands); the first two apply as they do to an ordinary projectile.

:::caution[A splitting projectile with no `AirburstWeapon` fires nothing]
When a splitting projectile with a [`Cluster`](/keys/cluster/) above `0` detonates, it releases no bomblets unless [`AirburstWeapon`](/keys/airburstweapon/) names a weapon with a `Projectile`. The `AirburstWeapon` page lists the settings that leave it without one.
:::
