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

We now wait for an enemy before a lightning storm that a Set Preferred Target Cell action aims fires, as we already do for the nuke. Before, the storm fired at the waypoint whether or not the house had an enemy.
