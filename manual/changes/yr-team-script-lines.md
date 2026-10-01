---
title: Carry out Yuri's Revenge team script lines
category: feature
release: 0.2.0
targets:
- type: key
  id: LeadershipRating
  effect: added
- type: key
  id: AISafeDistance
  effect: added
- type: mission
  id: TMISSION_GATHER_AT_ENEMY
  effect: added
- type: mission
  id: TMISSION_GATHER_AT_BASE
  effect: added
- type: mission
  id: TMISSION_IRON_CURTAIN_ME
  effect: added
- type: mission
  id: TMISSION_CHRONO_PREP_ABWP
  effect: added
- type: mission
  id: TMISSION_CHRONO_PREP_AQ
  effect: added
- type: mission
  id: TMISSION_MOVE_TO_OWN_BUILDING
  effect: added
- type: mission
  id: TMISSION_ATTACK_WAYPOINT_OBJECT
  effect: added
- type: mission
  id: TMISSION_ENTER_GRINDER
  effect: added
- type: mission
  id: TMISSION_ENTER_TANK_BUNKER
  effect: added
- type: mission
  id: TMISSION_ENTER_BIO_REACTOR
  effect: added
- type: mission
  id: TMISSION_ENTER_BATTLE_BUNKER
  effect: added
- type: mission
  id: TMISSION_GARRISON_STRUCTURE
  effect: added
credit: [Lucas]
---

Computer teams now carry out Yuri's Revenge's Gather at enemy base and Regroup at friendly base script lines, so their attack teams leave home instead of stalling. A team skips the other new lines, and any line it does not know, instead of stopping on it. A team's leader is now the member with the highest `LeadershipRating`.
