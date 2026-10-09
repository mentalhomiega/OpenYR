---
title: Give a TeamType with no TaskForce the first TaskForce an earlier TeamType names
category: fix
release: 0.2.0
targets:
- type: key
  id: TaskForce
  effect: changed
- type: action
  id: TACTION_REINFORCEMENTS
  effect: changed
- type: action
  id: TACTION_REINFORCEMENTS_SPECIAL
  effect: changed
- type: mission
  id: TMISSION_TEAMCHANGE
  effect: changed
- type: system
  id: ai-team-production
  effect: changed
credit: [MentalHomiega]
---

A TeamType with no `TaskForce=`, or with `<none>` or `none`, now takes the first TaskForce that an earlier TeamType names, as gamemd's TeamType loader does (`TeamTypeClass::LoadFromINI`, 0x006F1090). Before, it kept none, and making a team from it crashed the game. A TeamType with no TaskForce while no earlier TeamType has named one still keeps none and crashes the game.
