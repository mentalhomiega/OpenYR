---
title: Use up the wreckage count of a destroyed object
category: fix
release: 0.2.0
targets:
- type: key
  id: MaxDebris
  effect: changed
- type: key
  id: MinDebris
  effect: changed
- type: key
  id: DebrisMaximums
  effect: changed
credit: [MentalHomiega]
---

A destroyed object keeps throwing voxel wreckage until its count is used up, as Yuri's Revenge does. Before, it stopped after two whole passes in a row that threw nothing, and the rest of the count was lost. A `DebrisMaximums` figure below `0` now throws none; before, a figure of `-1` crashed the game. A `MinDebris` below `0` is read as `0`, and a `MaxDebris` below `MinDebris` is raised to `MinDebris`.
