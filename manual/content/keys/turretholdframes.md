---
key: TurretHoldFrames
summary: Game frames a structure's turret stays slid back before it recovers.
see_also: ["TurretRecoil", "TurretTravel", "TurretCompressFrames", "TurretRecoverFrames"]
when_omitted:
  kind: value
  value: "1"
---

`TurretHoldFrames` is the number of game frames the turret stays at its full recoil before it starts to return. A value of 1 or less starts the return on the frame the compression ends. It has an effect only when [`TurretRecoil=yes`](/keys/turretrecoil/) is set.
