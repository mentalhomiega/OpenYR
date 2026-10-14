---
title: Refuse a watching player the commands that play the match
category: fix
release: 0.2.0
targets:
- type: system
  id: observers
  effect: changed
- type: command
  id: PlaceBeacon
  effect: changed
- type: command
  id: NextObject
  effect: changed
- type: command
  id: PreviousObject
  effect: changed
- type: command
  id: CombatantSelect
  effect: changed
- type: command
  id: TypeSelect
  effect: changed
- type: command
  id: HealthFilter
  effect: changed
- type: command
  id: VeterancyFilter
  effect: changed
- type: command
  id: AllToCheer
  effect: changed
credit:
- MentalHomiega
---

A player who watches a multiplayer match, whether an observer or a defeated player, can no longer place a beacon, start planning mode, cycle the selection with the next and previous object keys, select by combatant, health, veterancy or type, or send the all-to-cheer. In a network match, a watcher's game speed change is no longer sent to the other players. A defeated player in coach mode keeps them.
