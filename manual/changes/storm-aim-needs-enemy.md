---
title: Wait for an enemy before a trigger-aimed storm fires
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
- type: action
  id: TACTION_SET_PREFERRED_TARGET_CELL
  effect: changed
credit:
- MentalHomiega
---

A lightning storm that a Set Preferred Target Cell action aims now waits until its house has an enemy, as the nuke does. Before, it fired at the waypoint whether or not the house had an enemy.
