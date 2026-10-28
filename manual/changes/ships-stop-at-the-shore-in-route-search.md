---
title: Keep beach cells out of a ship's movement zone
category: fix
release: 0.2.0
targets:
- type: enum
  id: MZoneType
  effect: changed
- type: system
  id: movement-and-terrain
  effect: changed
credit: [MentalHomiega]
---

A ship whose [`MovementZone=`](/keys/movementzone/) is `Water` is no longer routed over beach cells. Before, a long order could find no path because the route ran over a shoreline cell that the ship cannot enter. A `WaterBeach` type still crosses beach cells, and so do the amphibious classes. An `AmphibiousDestroyer` type now also accepts cells a terrain object partly fills.
