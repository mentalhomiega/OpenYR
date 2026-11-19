---
title: Add GpuVoxelSubpixel for smooth voxel motion in a zoomed-in view
category: feature
release: 0.2.0
targets:
- type: key
  id: GpuVoxelSubpixel
  effect: added
credit:
- MentalHomiega
---

[`GpuVoxelSubpixel=yes`](/keys/gpuvoxelsubpixel/) places the moving objects that `GpuVoxels` draws on the GPU by screen pixels instead of map pixels while the view is zoomed in, so a hover unit's bob and a slow move no longer go in steps. It is off by default.
