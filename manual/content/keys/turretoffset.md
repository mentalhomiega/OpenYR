---
key: TurretOffset
summary: Moves an object's turret forward from its center along the centerline.
see_also: ["PrimaryFireFLH", "SecondaryFireFLH", "Turret", "Voxel"]
when_omitted:
  kind: value
  value: "0"
---

The distance is in leptons, 256 to a cell. A positive value moves the turret toward the front, and a negative value moves it back.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
TurretOffset=-40 ; the turret sits behind the middle of the hull
```

The offset moves where shots come from. A vehicle's shots start the offset along its hull, and the firing offset of each weapon slot turns with the turret. Neither [`PrimaryFireFLH`](/keys/primaryfireflh/) nor [`SecondaryFireFLH`](/keys/secondaryfireflh/) has to repeat it. The mounting that every shot and effect starts from moves with it.

The offset also moves the drawn turret:

- A vehicle's voxel turret and barrel are drawn this far along the hull's heading, and turn about that point. The turret therefore pivots where its artwork sits, not at the middle of the hull. A vehicle's shots use the same point as the offset, so they leave from the turret's pivot plus the weapon's own offset.
- A structure's voxel turret ([`TurretAnimIsVoxel=yes`](/keys/turretanimisvoxel/)) is drawn this far along the turret's heading.
- An aircraft or an infantryman has no turret artwork, so only its firing offsets move.

:::caution[A structure's shots and its drawn turret can separate]
A structure's shots start the offset along its unturned axis, the direction 64 (`DIR_E`), but its voxel turret is drawn along the turret's heading. The two match only while the turret faces 64.
:::
