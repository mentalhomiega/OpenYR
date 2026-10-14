---
key: BarrelRecoverFrames
summary: Game frames a structure's barrel takes to return from its full recoil.
see_also: ["TurretRecoil", "BarrelTravel", "BarrelCompressFrames", "BarrelHoldFrames"]
when_omitted:
  kind: value
  value: "1"
---

`BarrelRecoverFrames` is the number of game frames the barrel takes to slide back to its rest position after holding. A value below 1 counts as 1. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set.
