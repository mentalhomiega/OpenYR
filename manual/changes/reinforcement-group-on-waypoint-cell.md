---
title: Place a reinforcement group on its waypoint's cell
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_REINFORCEMENTS
  effect: changed
- type: action
  id: TACTION_REINFORCEMENTS_SPECIAL
  effect: changed
- type: action
  id: TACTION_REINFORCEMENTS_CHRONO
  effect: changed
credit:
- MentalHomiega
---

A reinforcement group now appears on its waypoint's cell, facing its house's edge, unless its trigger names a waypoint inside the local radar area; then it appears near that waypoint. A group with no waypoint still takes an edge cell. Before, a team's own waypoint gave the edge cell, which could be a cell outside the radar where the group could not move.

This follows Yuri's Revenge's reinforcement routine (0x0065D8E0, which places the group through 0x0065E010). In the decompiled code, the edge search at 0x004AA440 is called only for aircraft, paradrops and retreats. On Soviet 1, the transports from waypoints I to L now reach waypoint 23 and unload their riders.
