---
title: Draw GPU voxels straight into the map view
category: feature
release: 0.2.0
targets:
- type: key
  id: GpuVoxels
  effect: changed
credit:
- MentalHomiega
---

[`GpuVoxels=yes`](/keys/gpuvoxels/) now draws each voxel object straight into the map view's picture, as geometry with a depth buffer, where it first drew the object into a shared voxel image. Layers and turret pieces hide each other by depth, a vehicle with a voxel turret shows highlights, and every object keeps the full [`GpuVoxelScale`](/keys/gpuvoxelscale/). `GpuVoxels` keeps its default.
