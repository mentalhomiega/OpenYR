---
key: BarrelAnimZAdjust
scope: buildingtype
label: Voxel barrel depth bias
summary: Sets the depth bias, in pixels, of a building's voxel barrel.
see_also: ["BarrelAnimIsVoxel", "TurretAnimZAdjust", "VoxelBarrelFile"]
when_omitted:
  kind: value
  value: "-21"
---

`BarrelAnimZAdjust` sets the depth bias, in pixels, that a building's voxel barrel is drawn with. It takes effect only on a building that also sets [`BarrelAnimIsVoxel=yes`](/keys/barrelanimisvoxel/). A lower value draws the barrel in front of what it overlaps, and a higher value draws it behind, as [`TurretAnimZAdjust`](/keys/turretanimzadjust/) does for a voxel turret. The default, `-21`, draws the barrel 21 pixels in front.

```ini title="rules.ini"
[MYARTILLERY] ; a BuildingType registered in [BuildingTypes]
BarrelAnimIsVoxel=yes
BarrelAnimZAdjust=-40
```

The value is read as an integer and is not clamped. It does not move the turret animation, which keeps the bias that [`TurretAnimZAdjust`](/keys/turretanimzadjust/) gives it.

A building that also sets [`TurretAnimIsVoxel=yes`](/keys/turretanimisvoxel/) ignores this key. Its voxel barrel takes the bias of `TurretAnimZAdjust` with its voxel turret.
