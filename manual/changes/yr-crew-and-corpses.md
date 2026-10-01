---
title: Read survivors and corpses as Yuri's Revenge does
category: fix
release: 0.2.0
targets:
- type: key
  id: Crew
  effect: removed
- type: key
  id: AlliedCrew
  effect: added
- type: key
  id: SovietCrew
  effect: added
- type: key
  id: ThirdCrew
  effect: added
- type: key
  id: DeadBodies
  scope: global-rules
  effect: changed
- type: key
  id: DeadBodies
  scope: infantrytype
  effect: added
- type: key
  id: NotHuman
  effect: added
credit: [Lucas]
---

Survivors now take their side's crew type from `AlliedCrew`, `SovietCrew` or `ThirdCrew`, and the shared `DeadBodies` list is read from `[General]`. An InfantryType can name its own `DeadBodies`, and `NotHuman=yes` keeps it from leaving a shared corpse. Selling or losing a structure no longer crashes for want of a crew type, and an infantry death no longer crashes on an empty corpse list.
