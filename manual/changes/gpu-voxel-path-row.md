---
title: Add a Draw voxels directly switch to the Graphics settings page
category: feature
release: 0.2.0
targets:
- type: key
  id: GpuVoxelPath
  effect: changed
credit:
- MentalHomiega
---

The **GPU voxels** group of the Graphics page has a **Draw voxels directly** switch for [`GpuVoxelPath`](/keys/gpuvoxelpath/). It is greyed until **Draw units on the GPU** is ticked. Ticking it writes `Direct`, and clearing it writes `Image`. The key, its default and its file format are unchanged.
