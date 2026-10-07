---
title: Run the Yuri's Revenge trigger actions the campaign uses
category: feature
release: 0.2.0
targets:
- type: action
  id: TACTION_BLACKOUT_RADAR
  effect: changed
- type: action
  id: TACTION_CENTER_BASE_CELL_CLEAR
  effect: changed
- type: action
  id: TACTION_CENTER_BASE_CELL_SET
  effect: changed
- type: action
  id: TACTION_CHRONO_SCREEN_EFFECT
  effect: changed
- type: action
  id: TACTION_CLEAR_PREFERRED_TARGET_CELL
  effect: changed
- type: action
  id: TACTION_CLEAR_SMUDGES
  effect: changed
- type: action
  id: TACTION_EVICT_OCCUPIERS
  effect: changed
- type: action
  id: TACTION_FLASH_BUILDINGS
  effect: changed
- type: action
  id: TACTION_FLASH_CAMEO
  effect: changed
- type: action
  id: TACTION_RESHROUD_AT
  effect: changed
- type: action
  id: TACTION_RESTORE_STARTING_BUILDINGS
  effect: changed
- type: action
  id: TACTION_SET_PREFERRED_TARGET_CELL
  effect: changed
- type: action
  id: TACTION_SET_SUPER_CHARGE
  effect: changed
- type: action
  id: TACTION_SET_TAB
  effect: changed
- type: action
  id: TACTION_STOP_SOUNDS_AT
  effect: changed
- type: action
  id: TACTION_SUPER_RESET
  effect: changed
- type: action
  id: TACTION_SUPER_RESET_RECHARGE_TIME
  effect: changed
- type: action
  id: TACTION_SUPER_SET_RECHARGE_TIME
  effect: changed
- type: action
  id: TACTION_TELEPORT_ALL_TO
  effect: changed
- type: action
  id: TACTION_TIMER_TEXT
  effect: changed
credit: [MentalHomiega]
---

We carry out the Yuri's Revenge trigger actions that its campaign maps use and that were skipped before: Reshroud Map At (101), Timer Text (103), Evict Occupiers (111), Set Tab to (114), Flash Cameo (115), Stop Sounds At (116), Clear all smudges (118), Chrono Screen Effect (127), Teleport All to (128), the four Superweapon charge actions (129, 132, 133 and 134), Restore Starting Buildings (130), Flash Buildings of Type (131), the preferred target cell actions (135 and 136) and the base center cell actions (137 and 138), and Blackout Radar (139).

Chrono Screen Effect only fades the picture to white and back, without the twirl or the input lock. A super weapon now keeps its own charge time, and a house keeps the cell its super weapons are aimed at, the center of its base, and the structures the scenario placed for it, and a scenario keeps the mission timer's label, so the save revision went up four times. The mission timer, which was drawn where the next frame painted over it, now shows at the bottom right with its label. Actions 102, 109, 110, 123, 124, 126 and 140 to 145 are still skipped, with a line in the log the first time one runs.
