---
title: Fire the Iron Curtain
category: feature
release: 0.2.0
targets:
- type: key
  id: Type
  scope: superweapontype
  effect: changed
- type: key
  id: Action
  scope: superweapontype
  effect: changed
- type: key
  id: IronCurtainDuration
  effect: added
- type: key
  id: IronCurtainInvokeAnim
  effect: added
- type: key
  id: AIMinorSuperReadyPercent
  effect: added
- type: key
  id: Organic
  effect: added
- type: mission
  id: TMISSION_IRON_CURTAIN_ME
  effect: changed
- type: system
  id: superweapons
  effect: changed
- type: enum
  id: ActionType
  effect: changed
credit: [MentalHomiega]
---

A `Type=IronCurtain` superweapon now protects the vehicles and structures around its target from damage for `IronCurtainDuration` frames and kills the infantry there, as in Yuri's Revenge. Computer teams fire it with the Iron Curtain me script line. The other Yuri's Revenge superweapon names and their targeting cursors are recognized, though their effects are not built yet.
