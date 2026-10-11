---
title: Start a house without a numbered waypoint inside the playable area
category: fix
release: 0.2.0
targets:
- type: system
  id: starting-forces
  effect: changed
credit: [MentalHomiega]
---

A house with no numbered start waypoint starts only where an 8-by-8 block of open ground lies wholly inside the playable area, and its start cell is three cells along on each axis from the block's first cell. Before, only the block's first cell was tested against the playable area, so a computer's base unit could start at the edge, where no deploy site fit, and take a long time to find one.
