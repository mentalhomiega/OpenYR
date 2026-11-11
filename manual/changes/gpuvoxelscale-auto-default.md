---
title: Take the GPU voxel sharpness from the zoom by default
category: feature
release: 0.2.0
targets:
- type: key
  id: GpuVoxelScale
  effect: changed
credit:
- MentalHomiega
---

[`GpuVoxelScale`](/keys/gpuvoxelscale/) accepts 0, which takes the sharpness from the map view's zoom, and 0 is the new default. The Graphics page shows it as **Auto**. Values 1 to 4 keep their meaning; set `GpuVoxelScale=1` for the earlier default.
