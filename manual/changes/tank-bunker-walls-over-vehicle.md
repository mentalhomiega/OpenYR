---
title: Tank Bunker walls over the vehicle
category: fix
release: 0.2.0
targets:
- type: system
  id: tank-bunkers
  effect: changed
- type: key
  id: OccupyHeight
  effect: changed
credit: [MentalHomiega]
---

A vehicle in a Tank Bunker is no longer drawn in front of the whole bunker. The bunker's front wall now draws over its tracks and lower hull, as in Yuri's Revenge, and only the turret and upper hull show above the walls. `OccupyHeight` no longer raises the vehicle's drawing.
