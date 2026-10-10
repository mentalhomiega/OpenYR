---
title: Place computer structures only on flat, clear ground
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
- type: key
  id: WantsExtraSpace
  effect: added
- type: key
  id: AIBaseSpacing
  effect: changed
credit:
- MentalHomiega
---

A computer house now places a structure only where the structure and its `AIBaseSpacing` border are flat and free of terrain objects, overlays, structures and blocked cells. Before, the border only had to be free of the house's own reserved ground, and a last pass dropped even that, so a war factory could end up against a tree, an ore field or a neighbour with its exit shut. The pass that dropped the border is removed. `ProtectWithWall` and the new `WantsExtraSpace` key ask for one more cell of border, and the search starts one cell further out on its south-facing sides. The save revision moves to 39, so saves from earlier builds are not compatible.
