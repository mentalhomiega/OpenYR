---
title: Run a waypoint path before attacking from it
category: fix
release: 0.2.0
targets:
- type: key
  id: WaypointAttackFromFirst
  effect: added
- type: system
  id: waypoint-paths
  effect: changed
credit: [MentalHomiega]
---

An attack click on a unit or building that stands on a waypoint now starts the selected units' path at its first waypoint. The units drive through the earlier waypoints and attack once they reach the attacked waypoint. Before, they attacked at once and skipped the moves before it. [`WaypointAttackFromFirst`](/keys/waypointattackfromfirst/) set to `no` restores the old behaviour.
