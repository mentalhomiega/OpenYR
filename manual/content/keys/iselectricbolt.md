---
key: IsElectricBolt
summary: Draws a jagged electric bolt from the muzzle to the target as the weapon fires.
see_also: ["IsAlternateColor", "IsLaser", "DefaultSparkSystem"]
when_omitted:
  kind: value
  value: "no"
---

`IsElectricBolt=yes` draws an electric bolt from the muzzle to the center of the target each time the weapon fires. The bolt is only a visual effect. The weapon still fires its projectile, and the projectile deals all the damage when it detonates.

```ini title="rulesmd.ini"
[MyCoilZap] ; example WeaponType
IsElectricBolt=yes
Projectile=InvisibleLow ; a BulletType, registered by a weapon naming it as its Projectile
```

The bolt is three jagged strands that arc upward between the two ends. Two strands use color 10 of `PALETTE.PAL`, or color 5 with [`IsAlternateColor=yes`](/keys/isalternatecolor/), and the third uses color 15. The strands take a new shape every frame, and the bolt lasts 17 frames. Objects and terrain in front of the bolt hide it.

When a vehicle fires, the start of its bolt follows the vehicle's muzzle while the bolt lasts. Only one bolt follows a vehicle at a time; a vehicle that fires again before its bolt fades leaves the second bolt where it started. The end of a bolt never moves.

Each shot also starts the [`DefaultSparkSystem`](/keys/defaultsparksystem/) particle system at the target.

A weapon that also sets [`IsLaser=yes`](/keys/islaser/) draws only the laser beam.
