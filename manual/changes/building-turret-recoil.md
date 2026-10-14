---
title: Recoil structure turrets and barrels only as their rules set
category: fix
release: 0.2.0
targets:
- type: key
  id: TurretRecoil
  effect: added
- type: key
  id: BarrelTravel
  effect: added
- type: key
  id: TurretTravel
  effect: added
credit:
- MentalHomiega
---

Every structure with a voxel turret jerked its turret back two pixels for one frame when it fired, including the Soviet Flak Cannon and the Yuri Gattling Cannon, which do not recoil. Now a structure recoils only when its rules set `TurretRecoil=yes`, as in Yuri's Revenge. Its turret body and barrel then slide back by their `TurretTravel` and `BarrelTravel` over the compress, hold and recover frames of each part. The Allied Grand Cannon's barrel slides back eight leptons, holds, and returns over 40 frames.
