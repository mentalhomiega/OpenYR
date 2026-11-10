---
title: Add GpuVoxelPath and GpuVoxelLayerDepth for drawing GPU voxels straight into the map view
category: feature
release: 0.2.0
targets:
- type: key
  id: GpuVoxelPath
  effect: added
- type: key
  id: GpuVoxelLayerDepth
  effect: added
credit:
- MentalHomiega
---

[`GpuVoxelPath=Direct`](/keys/gpuvoxelpath/) draws vehicles with or without a turret, aircraft and other plain voxel objects straight into the GPU map view, and [`GpuVoxelLayerDepth`](/keys/gpuvoxellayerdepth/) sets how the layers of such an object hide each other. The default `Image` changes nothing.
