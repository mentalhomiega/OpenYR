---
title: Add the voxel specular from view key
category: feature
release: 0.2.0
targets:
- type: key
  id: VoxelSpecularFromView
  effect: added
credit: [MentalHomiega]
---

`VoxelSpecularFromView=` in `[AudioVisual]` chooses where the viewer direction for the voxel highlight comes from. The default, `no`, uses the transform the voxel is drawn with, so the highlight follows the unit's facing. At `yes`, the viewer direction comes from the isometric view alone, so it stays fixed while the unit turns. The light direction is unchanged, and the key has no effect while the highlight is off.
