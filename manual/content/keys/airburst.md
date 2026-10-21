---
key: Airburst
summary: Holds a homing projectile at cruising height so that it goes off above its target rather than diving onto it.
see_also: [Splits, AirburstWeapon, Cluster, VeryHigh, ROT]
when_omitted:
  kind: value
  value: "no"
---

Only a homing projectile is affected: one whose [`ROT`](/keys/rot/#scope-bullettype) is above zero.

An ordinary homing projectile follows the terrain until its last three cells of approach, or six with [`VeryHigh=yes`](/keys/veryhigh/), and then pitches straight at its target. An airburst projectile never makes that dive. It keeps following the terrain however little distance is left, holding ten terrain levels of clearance above the ground ahead of it.

Terrain following needs a turn rate above `1`, and it does not run during the launch phase. A projectile at `ROT=1` under the stock [`MissileROTVar`](/keys/missilerotvar/) never follows the terrain, so it never holds that clearance. The other changes below still apply to it.

An airburst projectile measures the distance still to run on the horizontal alone. Every other homing projectile counts a quarter of the height difference toward that distance. An airburst projectile therefore counts as having arrived once it is over its target, not once it has reached it.

An airburst projectile goes off where it is. A projectile that arrives while still in the air is normally moved onto the target coordinate first, and one whose fuse trips close to its target is moved onto the victim. An airburst projectile skips both, so the blast happens overhead.

An airburst projectile is also exempt from the check that detonates a homing projectile once it stops gaining on its target, because it is meant to hang above its target.

A projectile chasing an aircraft never follows the terrain; it flies straight at its target. On that shot the clearance above does not apply, but the horizontal distance measure, the detonation changes and the exemption from the stall check all do.

When an airburst projectile goes off it fires its [`AirburstWeapon`](/keys/airburstweapon/) once at each of nine cells: the cell it burst over and its eight neighbours. [`Cluster`](/keys/cluster/) is not used, and the projectile detonates once. An airburst projectile that writes `Splits=yes` splits into `Cluster` bomblets instead, as [`Splits`](/keys/splits/) describes.

An airburst projectile whose `AirburstWeapon` is missing, or whose weapon has no [`Projectile`](/keys/projectile/), detonates but releases no bomblets.

```ini title="rules.ini"
[MYBURSTMISSILE] ; a BulletType, registered by a weapon naming it as its Projectile
Image=MISLMLTI
ROT=4
Airburst=yes
AirburstWeapon=MyBomblet ; a WeaponType that must also be listed in [Weapons]
```

The example omits the `MyBomblet` weapon section and its `[Weapons]` entry.
