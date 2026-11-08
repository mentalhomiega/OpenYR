---
title: Read turret recoil keys on every techno type
category: fix
release: 0.2.0
targets:
- type: key
  id: TurretRecoil
  effect: changed
- type: key
  id: TurretTravel
  effect: changed
- type: key
  id: BarrelTravel
  effect: changed
- type: key
  id: TurretCompressFrames
  effect: added
- type: key
  id: TurretHoldFrames
  effect: added
- type: key
  id: TurretRecoverFrames
  effect: added
- type: key
  id: BarrelCompressFrames
  effect: added
- type: key
  id: BarrelHoldFrames
  effect: added
- type: key
  id: BarrelRecoverFrames
  effect: added
- type: key
  id: TurretRecoil
  effect: removed
  scope: buildingtype
- type: key
  id: TurretRecoil
  effect: removed
  scope: unittype
credit:
- MentalHomiega
---

We read `TurretRecoil` and its travel and frame keys for every techno type, as Yuri's Revenge does. Before, only structure and vehicle types read them, and a vehicle's recoil was a one-frame jerk that ignored these keys.

A structure keeps its recoil. A vehicle whose body is an SHP image now slides its turret and barrel back over these keys. A vehicle with a voxel body does not move its turret, so the keys do nothing for it. Infantry and aircraft types read the keys, but nothing draws their recoil. A `Barrel` key that is left out takes the value of its `Turret` key.

Saves made by earlier builds no longer load, because structure, vehicle, infantry and aircraft types now save these settings in a new order.
