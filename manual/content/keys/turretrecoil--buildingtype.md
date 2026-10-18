---
key: TurretRecoil
scope: buildingtype
label: Structure turret recoil
see_also: ["TurretTravel", "BarrelTravel", "TurretAnimIsVoxel", "TurretCompressFrames", "BarrelCompressFrames"]
when_omitted:
  kind: value
  value: "no"
---

`TurretRecoil=yes` makes a structure's turret recoil each time it fires. Without it, the turret and barrel stay where they are when the structure fires.

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
