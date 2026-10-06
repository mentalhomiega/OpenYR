---
title: Keep 702 waypoints in a scenario
category: fix
release: 0.2.0
targets:
- type: key
  id: Waypoint
  effect: changed
- type: action
  id: TACTION_PLAY_SOUND_RANDOM
  effect: changed
credit:
- MentalHomiega
---

A scenario now keeps 702 waypoints, `A` through `ZZ`, as Yuri's Revenge does. It used to keep 101, so a map's waypoints numbered above 100 were not read, and a trigger, team or script naming one could read its cell from unrelated memory. All but one Yuri's Revenge campaign map use such waypoints, and the first Allied mission crashed as play began.

The [Play Sound Effect (Random)...](/mapping/actions/taction-play-sound-random/) action now picks from every placed waypoint and no longer corrupts memory when a scenario places more than 100 of them.
