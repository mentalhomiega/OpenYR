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

A reinforcement action for a TeamType with no `Script=` no longer crashes. As in Yuri's Revenge, we give the TeamType a new empty Script first. A Script with no missions then gets a Guard area mission with a timer of zero, which ends at once. We had given it an attack-waypoint mission, so reinforced teams no longer attack the waypoint.
