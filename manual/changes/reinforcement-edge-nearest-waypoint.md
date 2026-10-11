---
title: Enter from the map edge nearest the waypoint
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_REINFORCEMENTS
  effect: changed
- type: key
  id: Edge
  effect: changed
credit:
- MentalHomiega
---

The edge search for a reinforcement subtracted the playable area's origin twice, so it could pick an edge other than the nearest one. A group could then arrive on a cell it could not leave. The group now enters from the edge nearest its waypoint.
