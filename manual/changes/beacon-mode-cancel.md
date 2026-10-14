---
title: End beacon placement when another mode or action takes over
category: fix
release: 0.2.0
targets:
- type: command
  id: PlaceBeacon
  effect: changed
- type: command
  id: ToggleRepair
  effect: changed
- type: command
  id: ToggleSell
  effect: changed
- type: command
  id: TogglePower
  effect: changed
- type: command
  id: WaypointMode
  effect: changed
- type: command
  id: TeamSelect_1
  effect: changed
- type: command
  id: TeamSelect_2
  effect: changed
- type: command
  id: TeamSelect_3
  effect: changed
- type: command
  id: TeamSelect_4
  effect: changed
- type: command
  id: TeamSelect_5
  effect: changed
- type: command
  id: TeamSelect_6
  effect: changed
- type: command
  id: TeamSelect_7
  effect: changed
- type: command
  id: TeamSelect_8
  effect: changed
- type: command
  id: TeamSelect_9
  effect: changed
- type: command
  id: TeamSelect_10
  effect: changed
- type: command
  id: TeamAddSelect_1
  effect: changed
- type: command
  id: TeamAddSelect_2
  effect: changed
- type: command
  id: TeamAddSelect_3
  effect: changed
- type: command
  id: TeamAddSelect_4
  effect: changed
- type: command
  id: TeamAddSelect_5
  effect: changed
- type: command
  id: TeamAddSelect_6
  effect: changed
- type: command
  id: TeamAddSelect_7
  effect: changed
- type: command
  id: TeamAddSelect_8
  effect: changed
- type: command
  id: TeamAddSelect_9
  effect: changed
- type: command
  id: TeamAddSelect_10
  effect: changed
- type: command
  id: TeamCenter_1
  effect: changed
- type: command
  id: TeamCenter_2
  effect: changed
- type: command
  id: TeamCenter_3
  effect: changed
- type: command
  id: TeamCenter_4
  effect: changed
- type: command
  id: TeamCenter_5
  effect: changed
- type: command
  id: TeamCenter_6
  effect: changed
- type: command
  id: TeamCenter_7
  effect: changed
- type: command
  id: TeamCenter_8
  effect: changed
- type: command
  id: TeamCenter_9
  effect: changed
- type: command
  id: TeamCenter_10
  effect: changed
- type: action
  id: TACTION_LOCK_INPUT
  effect: changed
- type: system
  id: sidebar
  effect: changed
credit:
- MentalHomiega
---

Before, starting repair, sell, power or waypoint mode, picking a super weapon target or a building to place, selecting, adding or centering a team, locking input, or the player's defeat left beacon placement on, so the next map click could still place a beacon. Now each of those ends beacon placement, as in Yuri's Revenge. Starting beacon placement already ended the other modes.
