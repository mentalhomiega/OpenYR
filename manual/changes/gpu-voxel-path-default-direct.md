---
title: Draw GPU voxels directly by default
category: feature
release: 0.2.0
targets:
- type: key
  id: GpuVoxelPath
  effect: changed
credit:
- MentalHomiega
---

[`GpuVoxelPath`](/keys/gpuvoxelpath/) now defaults to `Direct`, so [`GpuVoxels=yes`](/keys/gpuvoxels/) draws voxel objects straight into the map view unless `GpuVoxelPath=Image` is set. `GpuVoxels` keeps its default.
