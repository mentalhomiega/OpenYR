---
title: Fly Droppod teams in aboard a paradrop plane
category: fix
release: 0.2.0
targets:
- type: key
  id: Droppod
  effect: changed
- type: key
  id: UseTransportOrigin
  effect: added
- type: key
  id: TransportWaypoint
  effect: added
credit: [MentalHomiega]
---

A `Droppod=yes` reinforcement team landed by drop pod, and only when it was all infantry. As in Yuri's Revenge, it now flies in aboard a `PDPLANE` with every member aboard and parachutes them over its waypoint. `UseTransportOrigin` and `TransportWaypoint` are read, so the plane can start at a waypoint instead of the map edge.
