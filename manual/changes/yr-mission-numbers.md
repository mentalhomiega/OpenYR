---
title: Read mission numbers in Yuri's Revenge order
category: fix
release: 0.2.0
targets:
- type: mission
  id: TMISSION_DO
  effect: changed
- type: action
  id: TACTION_ALL_ASSIGN_MISSION
  effect: changed
- type: enum
  id: MissionType
  effect: changed
credit: [MentalHomiega]
---

Missions are now numbered as in Yuri's Revenge, with Eaten, Wait and Attack Move in the list, so team script Do lines and the All Assign Mission trigger action give the mission the game data names: `11` gives Area Guard instead of Return, and `15` gives Hunt.
