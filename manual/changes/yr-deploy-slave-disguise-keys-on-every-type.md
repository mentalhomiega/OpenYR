---
title: Read DeployToLand, DisguiseWhenStill, Slaved and UndeployDelay on every object type
category: fix
release: 0.2.0
targets:
- type: key
  id: DeployToLand
  effect: added
  scope: aircrafttype
- type: key
  id: DeployToLand
  effect: removed
  scope: unittype
- type: key
  id: DisguiseWhenStill
  effect: added
  scope: aircrafttype
- type: key
  id: DisguiseWhenStill
  effect: removed
  scope: unittype
- type: key
  id: Slaved
  effect: added
  scope: aircrafttype
- type: key
  id: Slaved
  effect: removed
  scope: infantrytype
- type: key
  id: UndeployDelay
  effect: added
  scope: aircrafttype
- type: key
  id: UndeployDelay
  effect: removed
  scope: infantrytype
credit:
- MentalHomiega
---

We now read `DeployToLand`, `DisguiseWhenStill`, `Slaved` and `UndeployDelay` on every object type, as Yuri's Revenge does. Only vehicles use `DisguiseWhenStill`, and only soldiers use `Slaved` and `UndeployDelay`, so those keys change nothing else. An aircraft or soldier with `DeployToLand=yes` that is above the ground now advances its walk counter on every frame, so an aircraft's body animation advances every frame while it is airborne.
