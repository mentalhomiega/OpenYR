---
key: TurretCompressFrames
summary: Game frames a turret takes to slide back to its full recoil, for a structure or SHP-bodied vehicle.
see_also: ["TurretRecoil", "TurretTravel", "TurretHoldFrames", "TurretRecoverFrames"]
when_omitted:
  kind: value
  value: "1"
---

`TurretCompressFrames` is the number of game frames the turret takes to slide back its full [`TurretTravel`](/keys/turrettravel/) after a structure or SHP-bodied vehicle fires. A value below 1 counts as 1. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set.
