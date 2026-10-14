---
title: Add the voxel specular strength key
category: feature
release: 0.2.0
targets:
- type: key
  id: VoxelSpecularStrength
  effect: added
credit: [MentalHomiega]
---

`VoxelSpecularStrength=` in `[AudioVisual]` sets how strong the highlight is on units, aircraft and buildings drawn as voxels. The default, `3`, is the value those voxels now ship with: before this change they took no highlight, and now they do. A value of `0` or less turns the highlight off. Bullets and voxel animations never take one.
