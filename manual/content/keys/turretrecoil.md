---
key: TurretRecoil
summary: Makes a voxel turret and barrel slide back when a structure or an SHP-bodied vehicle fires.
see_also: ["TurretTravel", "BarrelTravel", "TurretAnimIsVoxel", "TurretCompressFrames", "BarrelCompressFrames"]
when_omitted:
  kind: value
  value: "no"
---

`TurretRecoil=yes` makes the turret body and barrel slide back each time the object fires. Without it, both parts stay where they are when the object fires.

A structure recoils when its turret is a voxel. A vehicle recoils only when its body is an SHP image. A vehicle with a voxel body does not move its turret when it fires, even with `TurretRecoil=yes`. Infantry and aircraft types read the setting, but nothing draws their recoil.

Each shot starts both parts moving back at once. The turret body then moves by [`TurretTravel`](/keys/turrettravel/) and the barrel by [`BarrelTravel`](/keys/barreltravel/), each over its own compress, hold and recover frames: [`TurretCompressFrames`](/keys/turretcompressframes/), [`TurretHoldFrames`](/keys/turretholdframes/) and [`TurretRecoverFrames`](/keys/turretrecoverframes/) for the turret body, and the `Barrel` counterparts for the barrel. A part with zero travel does not move.

```ini title="rules.ini"
[GTGCAN] ; a BuildingType with a voxel turret
TurretRecoil=yes
TurretTravel=0 ; the turret body stays in place
BarrelTravel=8 ; the barrel slides back 8 leptons
BarrelCompressFrames=3
BarrelHoldFrames=3
BarrelRecoverFrames=40
```

The recoil moves the drawn turret and barrel only. It does not change where shots start.

A save taken mid-recoil loads with both parts at rest.
