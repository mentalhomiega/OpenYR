---
title: Add GpuVoxelScale for sharper GPU voxel drawing
category: feature
release: 0.2.0
targets:
- type: key
  id: GpuVoxelScale
  effect: added
credit:
- MentalHomiega
---

[`GpuVoxelScale`](/keys/gpuvoxelscale/) draws the map view at two to four times its pixel size when `GpuVoxels=yes`, so voxel objects stay sharp in a zoomed map view. The default 1 changes nothing.
