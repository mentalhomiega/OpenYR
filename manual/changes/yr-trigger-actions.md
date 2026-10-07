---
title: Run Yuri's Revenge's trigger actions
category: feature
release: 0.2.0
targets:
- type: action
  id: TACTION_BLACKOUT_RADAR
  effect: added
- type: action
  id: TACTION_CENTER_BASE_CELL_CLEAR
  effect: added
- type: action
  id: TACTION_CENTER_BASE_CELL_SET
  effect: added
- type: action
  id: TACTION_CHEER
  effect: added
- type: action
  id: TACTION_CHRONO_SCREEN_EFFECT
  effect: added
- type: action
  id: TACTION_CLEAR_DEFENSIVE_TARGET_CELL
  effect: added
- type: action
  id: TACTION_CLEAR_PREFERRED_TARGET_CELL
  effect: added
- type: action
  id: TACTION_CLEAR_SMUDGES
  effect: added
- type: action
  id: TACTION_CREATE_BUILDING
  effect: added
- type: action
  id: TACTION_CREATE_CRATE
  effect: added
- type: action
  id: TACTION_DESTROY_ALL
  effect: added
- type: action
  id: TACTION_DESTROY_ALL_BUILDINGS
  effect: added
- type: action
  id: TACTION_DESTROY_ALL_LAND_UNITS
  effect: added
- type: action
  id: TACTION_DESTROY_ALL_NAVAL_UNITS
  effect: added
- type: action
  id: TACTION_EVICT_OCCUPIERS
  effect: added
- type: action
  id: TACTION_FLASH_BUILDINGS
  effect: added
- type: action
  id: TACTION_FLASH_CAMEO
  effect: added
- type: action
  id: TACTION_IRON_CURTAIN_AT
  effect: added
- type: action
  id: TACTION_JUMP_CAMERA
  effect: added
- type: action
  id: TACTION_JUMP_CAMERA_HOME
  effect: added
- type: action
  id: TACTION_LIGHTNING_STORM_STRIKE
  effect: added
- type: action
  id: TACTION_MIND_CONTROL_BASE
  effect: added
- type: action
  id: TACTION_PAUSE_GAME
  effect: added
- type: action
  id: TACTION_PLAY_INGAME_MOVIE_PAUSED
  effect: added
- type: action
  id: TACTION_REINFORCEMENTS_CHRONO
  effect: added
- type: action
  id: TACTION_RESHROUD_AT
  effect: added
- type: action
  id: TACTION_RESTORE_MIND_CONTROLLED_BASE
  effect: added
- type: action
  id: TACTION_RESTORE_STARTING_BUILDINGS
  effect: added
- type: action
  id: TACTION_RESTORE_STARTING_UNITS
  effect: added
- type: action
  id: TACTION_RETINT_BLUE
  effect: added
- type: action
  id: TACTION_RETINT_GREEN
  effect: added
- type: action
  id: TACTION_RETINT_RED
  effect: added
- type: action
  id: TACTION_SET_DEFENSIVE_TARGET_CELL
  effect: added
- type: action
  id: TACTION_SET_PREFERRED_TARGET_CELL
  effect: added
- type: action
  id: TACTION_SET_SUPER_CHARGE
  effect: added
- type: action
  id: TACTION_SET_TAB
  effect: added
- type: action
  id: TACTION_SET_TECH_LEVEL
  effect: added
- type: action
  id: TACTION_STOP_SOUNDS_AT
  effect: added
- type: action
  id: TACTION_SUPER_RESET
  effect: added
- type: action
  id: TACTION_SUPER_RESET_RECHARGE_TIME
  effect: added
- type: action
  id: TACTION_SUPER_SET_RECHARGE_TIME
  effect: added
- type: action
  id: TACTION_TELEPORT_ALL_TO
  effect: added
- type: action
  id: TACTION_TIMER_TEXT
  effect: added
credit: [MentalHomiega]
---

Maps gain Yuri's Revenge's trigger actions 101 to 145. Make house cheer, Destroy all of and the other Destroy all actions, Create Building At, Center (Jump) Camera at Waypoint, Set Object's Tech Level, Create Crate and Reinforcement by Chrono are carried out; Reinforcement by Chrono brings the team in without the chronoshift effect. The rest are skipped, with a line in the log the first time one runs.
