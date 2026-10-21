---
key: BarrelHoldFrames
summary: Game frames a barrel stays slid back before it recovers, for a structure or SHP-bodied vehicle.
see_also: ["TurretRecoil", "BarrelTravel", "BarrelCompressFrames", "BarrelRecoverFrames"]
when_omitted:
  kind: inherited
  note: The TurretHoldFrames of the same type, which is 1 when that is omitted too.
---

`BarrelHoldFrames` is the number of game frames the barrel stays at its full recoil before it starts to return. A value of 1 or less starts the return on the frame the compression ends. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set.
