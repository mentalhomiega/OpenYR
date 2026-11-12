---
title: Remove GpuVoxelPath and the voxel image route
category: feature
release: 0.2.0
targets:
- type: key
  id: GpuVoxelPath
  effect: removed
credit:
- MentalHomiega
---

[`GpuVoxels=yes`](/keys/gpuvoxels/) now always draws voxel objects straight into the map view, as `GpuVoxelPath=Direct` did. `GpuVoxelPath` and the **Draw voxels directly** switch are gone, and a `GpuVoxelPath=` line in `RA2MD.INI` is ignored.
