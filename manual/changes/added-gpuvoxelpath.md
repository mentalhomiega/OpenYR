---
title: Add GpuVoxelLayerDepth for how the layers of a GPU voxel object hide each other
category: feature
release: 0.2.0
targets:
- type: key
  id: GpuVoxelLayerDepth
  effect: added
credit:
- MentalHomiega
---

[`GpuVoxelLayerDepth`](/keys/gpuvoxellayerdepth/) sets how the voxel layers of an object that [`GpuVoxels=yes`](/keys/gpuvoxels/) draws hide each other: by depth with the default `Shared`, or in the software's drawing order with `Ordered`.
