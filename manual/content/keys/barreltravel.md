---
key: BarrelTravel
summary: How far a structure's barrel slides back along its axis when it fires, in leptons.
see_also: ["TurretRecoil", "TurretTravel", "BarrelCompressFrames"]
when_omitted:
  kind: value
  value: "2"
---

`BarrelTravel` is the distance the barrel of a voxel turret slides back along its pitched axis when the structure fires, in leptons. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set. Zero keeps the barrel still.
