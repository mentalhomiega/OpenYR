---
key: BarrelTravel
summary: How far a barrel slides back along the turret's facing when a structure or SHP-bodied vehicle fires, in leptons.
see_also: ["TurretRecoil", "TurretTravel", "BarrelCompressFrames"]
when_omitted:
  kind: inherited
  note: The TurretTravel of the same type, which is 2 when that is omitted too.
---

`BarrelTravel` is the distance the barrel of a voxel turret slides back along the turret's facing, before the barrel pitches, when a structure or SHP-bodied vehicle fires, in leptons. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set. Zero keeps the barrel still.
