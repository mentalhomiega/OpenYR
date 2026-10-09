---
title: Stop reinforcements of a TeamType with no Script from crashing
category: fix
release: 0.2.0
targets:
- type: key
  id: Script
  effect: changed
- type: action
  id: TACTION_REINFORCEMENTS
  effect: changed
- type: action
  id: TACTION_REINFORCEMENTS_SPECIAL
  effect: changed
credit: [MentalHomiega]
---

A reinforcement action for a TeamType with no `Script=` no longer crashes. When the TeamType has no Script, the action gives it a new empty one first, as Yuri's Revenge does when it reinforces a team (0x0065D8E0). A Script with no missions then gets a Guard area mission with a timer of zero, which ends at once. We had given it an attack-waypoint mission, so reinforced teams no longer attack the waypoint. A TeamType normally has a Script when it loads, from the first Script an earlier TeamType names, so the new empty Script appears only for a TeamType that has none then.
