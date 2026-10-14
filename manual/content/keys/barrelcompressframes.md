---
key: BarrelCompressFrames
summary: Game frames a structure's barrel takes to slide back to its full recoil.
see_also: ["TurretRecoil", "BarrelTravel", "BarrelHoldFrames", "BarrelRecoverFrames"]
when_omitted:
  kind: value
  value: "1"
---

`BarrelCompressFrames` is the number of game frames the barrel takes to slide back its full [`BarrelTravel`](/keys/barreltravel/) after the structure fires. A value below 1 counts as 1. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set.
