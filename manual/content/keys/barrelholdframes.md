---
key: BarrelHoldFrames
summary: Game frames a structure's barrel stays slid back before it recovers.
see_also: ["TurretRecoil", "BarrelTravel", "BarrelCompressFrames", "BarrelRecoverFrames"]
when_omitted:
  kind: value
  value: "1"
---

`BarrelHoldFrames` is the number of game frames the barrel stays at its full recoil before it starts to return. A value of 1 or less starts the return on the frame the compression ends. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set.
