---
key: BarrelRecoverFrames
summary: Game frames a barrel takes to return from its full recoil, for a structure or SHP-bodied vehicle.
see_also: ["TurretRecoil", "BarrelTravel", "BarrelCompressFrames", "BarrelHoldFrames"]
when_omitted:
  kind: inherited
  note: The TurretRecoverFrames of the same type, which is 1 when that is omitted too.
---

`BarrelRecoverFrames` is the number of game frames the barrel takes to slide back to its rest position after holding. A value below 1 counts as 1. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set.
