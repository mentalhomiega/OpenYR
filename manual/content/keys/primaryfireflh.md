---
key: PrimaryFireFLH
summary: The offset from an object's center that its first weapon fires from.
see_also: ["Primary", "ElitePrimaryFireFLH", "PBarrelLength", "PBarrelThickness", "TurretOffset", "SecondaryFireFLH"]
when_omitted:
  kind: value
  value: 0,0,0
---

The value is three whole numbers, `X,Y,Z`, in leptons (256 to a cell). The offset turns with the gun as the object aims:

- `X` runs forward, in the direction the gun faces.
- `Y` runs sideways, with positive values to the gun's left.
- `Z` runs upward.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
PrimaryFireFLH=100,0,60 ; a bit under half a cell forward, 60 leptons up
```

The gun's heading depends on the kind of object. [Firing geometry](/systems/firing-geometry/#the-mounting) lists it for each.

## The mounting

The offset places one point, the mounting. It is where the projectile is created and where its aim is measured from. Most parts of a shot start there too, as [Firing geometry](/systems/firing-geometry/#what-each-part-of-a-shot-reads) lists. It is this offset with [`TurretOffset`](/keys/turretoffset/) added to `X`. It stays in place as the barrel elevates. On a vehicle or aircraft it also tilts with the body on slopes and in flight.

An infantry type fires from the same point. A passenger of an open-topped transport fires from its seat on the transport instead.

`X` and `Z` also position a drawn voxel barrel, including the point it pivots around as it elevates. Changing this offset can therefore move the barrel artwork as well as the shot.

## Burst weapons

`Y` is reversed on every second shot of a [`Burst`](/keys/burst/) weapon. A nonzero `Y` therefore gives a burst weapon two mountings, one on each side of the centerline. [How a burst alternates sides](/systems/firing-geometry/#how-a-burst-alternates-sides) covers the order and its exceptions.

## Other slots and overrides

The elite primary slot reads its own offset from [`ElitePrimaryFireFLH`](/keys/eliteprimaryfireflh/), which defaults to this value.

Two structure settings replace this offset for every weapon slot. If both are set, the first one listed applies:

- A [`PrimaryFirePixelOffset`](/keys/primaryfirepixeloffset/) other than `65535,65535` pins the mounting to that screen offset. With [`PrimaryFireDualOffset=yes`](/keys/primaryfiredualoffset/) the mounting is this offset with the pixel offset added.
- A [`BarrelAnimIsVoxel=yes`](/keys/barrelanimisvoxel/) structure takes the mounting from [`VoxelBarrelOffsetToBarrelEnd`](/keys/voxelbarreloffsettobarrelend/).

Otherwise, a structure whose turret is a voxel adds the turret animation's screen offset to the mounting.

A type with [`TurretCount`](/keys/turretcount/) above `0` ignores this key and reads its weapons from a [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) instead.
