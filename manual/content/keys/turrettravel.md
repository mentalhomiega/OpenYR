---
key: TurretTravel
summary: How far a turret body slides back when a structure or SHP-bodied vehicle fires, in leptons.
see_also: ["TurretRecoil", "BarrelTravel", "TurretCompressFrames"]
when_omitted:
  kind: value
  value: "2"
---

`TurretTravel` is the distance the turret body slides back along its facing when a structure or SHP-bodied vehicle fires, in leptons, 256 to a cell. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set. Zero keeps the turret body still.
