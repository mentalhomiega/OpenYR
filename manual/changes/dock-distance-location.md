---
title: Measure dock distance from the structure's location
category: fix
release: 0.2.0
targets:
- type: key
  id: Dock
  effect: changed
credit: [MentalHomiega]
---

A unit that chooses between several dock types measures the distance to each structure from the structure's location, not from its center. A primary structure wins over a nearer one of another dock type, as it already did within one type. Distances past about 181 cells wrap, as they do in gamemd.
