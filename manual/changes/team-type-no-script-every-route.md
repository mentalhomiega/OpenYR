---
title: Give a TeamType with no Script the first Script an earlier TeamType names
category: fix
release: 0.2.0
targets:
- type: key
  id: Script
  effect: changed
- type: action
  id: TACTION_CREATE_TEAM
  effect: changed
- type: mission
  id: TMISSION_TEAMCHANGE
  effect: changed
credit: [MentalHomiega]
---

A TeamType with no `Script=` now takes the first Script that an earlier TeamType names, as gamemd's TeamType loader does (`TeamTypeClass::LoadFromINI`, 0x006F1090). Before, it got a Guard-only Script the first time a team of it was made. A TeamType with no `TaskForce=` keeps no Script while no earlier TeamType has named a TaskForce, because gamemd's loader returns before it reaches the Script. A TeamType that still has no Script crashes the game on its team's first mission, as gamemd does. The one exception is a scenario in which no Script is registered at all: there each such TeamType gets a Guard-only Script at load, so its teams are disbanded at once.
