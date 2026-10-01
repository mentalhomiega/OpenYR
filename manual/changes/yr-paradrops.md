---
title: Fire the paradrops and the spy plane
category: feature
release: 0.2.0
targets:
- type: key
  id: Type
  scope: superweapontype
  effect: changed
- type: key
  id: AllyParaDropInf
  effect: added
- type: key
  id: AllyParaDropNum
  effect: added
- type: key
  id: SovParaDropInf
  effect: added
- type: key
  id: SovParaDropNum
  effect: added
- type: key
  id: YuriParaDropInf
  effect: added
- type: key
  id: YuriParaDropNum
  effect: added
- type: key
  id: AmerParaDropInf
  effect: added
- type: key
  id: AmerParaDropNum
  effect: added
- type: key
  id: ParadropRadius
  effect: added
- type: key
  id: SpyPlaneCamera
  effect: added
- type: key
  id: SpyPlaneCameraFrames
  effect: added
- type: key
  id: Parachute
  effect: changed
- type: key
  id: BombParachute
  effect: changed
- type: enum
  id: MissionType
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit: [Lucas]
---

The paradrop, American paradrop and spy plane superweapons now work as in Yuri's Revenge, and computer houses fire them. Aircraft on `Retreat` now fly off the map, `Parachute` is also read from `[General]`, and a missing canopy no longer crashes the game.
