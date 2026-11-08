---
key: TurretRecoverFrames
summary: Game frames a turret takes to return from its full recoil, for a structure or SHP-bodied vehicle.
see_also: ["TurretRecoil", "TurretTravel", "TurretCompressFrames", "TurretHoldFrames"]
when_omitted:
  kind: value
  value: "1"
---

`TurretRecoverFrames` is the number of game frames the turret takes to slide back to its rest position after holding. A value below 1 counts as 1. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set.
