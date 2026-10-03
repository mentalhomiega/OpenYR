---
title: Fire the chronosphere
category: feature
release: 0.2.0
targets:
- type: key
  id: Type
  scope: superweapontype
  effect: changed
- type: key
  id: PreClick
  effect: added
- type: key
  id: PostClick
  effect: added
- type: key
  id: PreDependent
  effect: added
- type: key
  id: ChronoPlacement
  effect: added
- type: key
  id: ChronoBlast
  effect: added
- type: key
  id: ChronoBlastDest
  effect: added
- type: key
  id: WarpOut
  effect: added
- type: key
  id: ChronoInSound
  effect: added
- type: key
  id: ChronoOutSound
  effect: added
- type: key
  id: Teleporter
  scope: aircrafttype
  effect: added
- type: key
  id: Teleporter
  scope: unittype
  effect: removed
- type: key
  id: Organic
  effect: changed
- type: system
  id: superweapons
  effect: changed
credit: [MentalHomiega]
---

The chronosphere now works as in Yuri's Revenge: one click picks the units and a second click warps them, killing organic units and crushing what stands where they land. `Teleporter=` is read for every object type, and infantry are `Organic=yes` unless their type says otherwise.
