---
key: TurretCompressFrames
summary: Game frames a structure's turret takes to slide back to its full recoil.
see_also: ["TurretRecoil", "TurretTravel", "TurretHoldFrames", "TurretRecoverFrames"]
when_omitted:
  kind: value
  value: "1"
---

`TurretCompressFrames` is the number of game frames the turret takes to slide back its full [`TurretTravel`](/keys/turrettravel/) after the structure fires. A value below 1 counts as 1. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set.
