---
title: Read DeployFire and GapGenerator on every object type
category: fix
release: 0.2.0
targets:
- type: key
  id: DeployFire
  effect: added
  scope: aircrafttype
- type: key
  id: DeployFire
  effect: removed
  scope: infantrytype
- type: key
  id: DeployFire
  effect: removed
  scope: unittype
- type: key
  id: DeployFireWeapon
  effect: added
  scope: aircrafttype
- type: key
  id: DeployFireWeapon
  effect: removed
  scope: infantrytype
- type: key
  id: DeployFireWeapon
  effect: removed
  scope: unittype
- type: key
  id: GapGenerator
  effect: added
  scope: aircrafttype
- type: key
  id: GapGenerator
  effect: removed
  scope: buildingtype
credit:
- MentalHomiega
---

We now read `DeployFire` and `DeployFireWeapon` on aircraft and structures, and `GapGenerator` on every object type, as Yuri's Revenge does. Only soldiers and simple deployers use `DeployFire`, and only structures use `GapGenerator`, so the new values change nothing else.
