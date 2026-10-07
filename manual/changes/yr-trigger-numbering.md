---
title: Number trigger actions and events as Yuri's Revenge does
category: feature
release: 0.2.0
targets:
- type: action
  id: TACTION_CREATE_AUTOSAVE
  effect: changed
- type: action
  id: TACTION_CREATE_BUILDING_AT
  effect: changed
- type: action
  id: TACTION_DELETE_OBJECT
  effect: changed
- type: action
  id: TACTION_DISABLE_ALLY_REVEAL
  effect: changed
- type: action
  id: TACTION_DISABLE_SHORT_GAME
  effect: changed
- type: action
  id: TACTION_DISABLE_SPEECH
  effect: changed
- type: action
  id: TACTION_ENABLE_ALLY_REVEAL
  effect: changed
- type: action
  id: TACTION_ENABLE_SHORT_GAME
  effect: changed
- type: action
  id: TACTION_ENABLE_SPEECH
  effect: changed
- type: action
  id: TACTION_FLASH_TEAM
  effect: changed
- type: action
  id: TACTION_GIVE_CREDITS
  effect: changed
- type: action
  id: TACTION_HOUSE_DESTROY_ALL
  effect: changed
- type: action
  id: TACTION_MAKE_ALLY_ONE_WAY
  effect: changed
- type: action
  id: TACTION_MAKE_ELITE
  effect: changed
- type: action
  id: TACTION_MAKE_ENEMY_ONE_WAY
  effect: changed
- type: action
  id: TACTION_SET_GROUP_ID
  effect: changed
- type: event
  id: TEVENT_ENEMY_IN_SPOTLIGHT_REPEATING
  effect: changed
- type: event
  id: TEVENT_LIMPED
  effect: changed
- type: event
  id: TEVENT_PARALYZED
  effect: changed
credit: [MentalHomiega]
---

A Yuri's Revenge map now runs the trigger action and event it names: actions 101 to 145 and events 53 to 61 are Yuri's Revenge's own. The extra OpenTS actions that stood at 101 to 118 now follow Yuri's Revenge's range at 146 to 161, and the extra events that stood at 53 to 55 follow at 62 to 64. Maps that use an extra action by its old number must use the new one.
