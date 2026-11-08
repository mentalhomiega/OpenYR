---
key: BarrelCompressFrames
summary: Game frames a barrel takes to slide back to its full recoil, for a structure or SHP-bodied vehicle.
see_also: ["TurretRecoil", "BarrelTravel", "BarrelHoldFrames", "BarrelRecoverFrames"]
when_omitted:
  kind: inherited
  note: The TurretCompressFrames of the same type, which is 1 when that is omitted too.
---

`BarrelCompressFrames` is the number of game frames the barrel takes to slide back its full [`BarrelTravel`](/keys/barreltravel/) after a structure or SHP-bodied vehicle fires. A value below 1 counts as 1. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set.
