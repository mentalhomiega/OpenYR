---
title: Give each command Yuri's Revenge's default key
category: feature
release: 0.2.0
targets:
- type: format
  id: keyboard-ini
  effect: changed
- type: command
  id: DefenseTab
  effect: changed
- type: command
  id: InfantryTab
  effect: changed
- type: command
  id: PlanningMode
  effect: changed
- type: command
  id: StructureTab
  effect: changed
- type: command
  id: UnitTab
  effect: changed
- type: command
  id: TypeSelect
  effect: added
- type: command
  id: CombatantSelect
  effect: added
- type: command
  id: HealthNav
  effect: added
- type: command
  id: VeterancyNav
  effect: added
- type: command
  id: PlaceBeacon
  effect: added
- type: command
  id: Delete
  effect: added
- type: command
  id: Taunt_1
  effect: added
- type: command
  id: Taunt_2
  effect: added
- type: command
  id: Taunt_3
  effect: added
- type: command
  id: Taunt_4
  effect: added
- type: command
  id: Taunt_5
  effect: added
- type: command
  id: Taunt_6
  effect: added
- type: command
  id: Taunt_7
  effect: added
- type: command
  id: Taunt_8
  effect: added
credit: [MentalHomiega]
---

A new player has Yuri's Revenge's key for each command without binding it. The five tab and planning commands gain their keys, and the new commands are registered. `TypeSelect`, `CombatantSelect` and `VeterancyNav` work. `PlaceBeacon` and `Delete` place and remove beacons in a multiplayer game, and the taunts report that the feature is not in this build. A keyboard file from an earlier build keeps its own keys, and it gains the keys of the commands it omits.
