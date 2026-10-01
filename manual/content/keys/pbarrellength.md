---
key: PBarrelLength
summary: How far out along the first weapon's barrel its muzzle sits.
see_also: ["PrimaryFireFLH", "PBarrelThickness", "ElitePBarrelLength", "SBarrelLength"]
when_omitted:
  kind: value
  value: "0"
---

The muzzle sits this many leptons, 256 to a cell, along the barrel from the mounting point that [`PrimaryFireFLH`](/keys/primaryfireflh/) sets. The distance follows the barrel's pitch, so the muzzle rises and falls as the barrel elevates while the mounting point stays put.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
PrimaryFireFLH=100,0,60
PBarrelLength=80 ; the muzzle sits 80 leptons further out along the barrel
```

The weapon's fire animation, laser beam, sonic wave and attached particle systems start at the muzzle. A structure that sets [`PrimaryFirePixelOffset`](/keys/primaryfirepixeloffset/) or [`BarrelAnimIsVoxel=yes`](/keys/barrelanimisvoxel/) takes its firing point from that setting instead, and ignores this one. The projectile starts at the muzzle only for infantry. An aircraft, structure or vehicle creates it at the mounting point that `PrimaryFireFLH` and [`TurretOffset`](/keys/turretoffset/) set.

A value above `0` also advances the weapon's projectile up to two steps along its flight as soon as it is fired. The second step is skipped if the first ends the projectile. Laser weapons and [`Inviso=yes`](/keys/inviso/) projectiles are not advanced.

The elite primary slot uses [`ElitePBarrelLength`](/keys/elitepbarrellength/), which defaults to this value.

A type with [`TurretCount`](/keys/turretcount/) above `0` ignores this key. The weapons of its [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) fire with no barrel length or thickness.
