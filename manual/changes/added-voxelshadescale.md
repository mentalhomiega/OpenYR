---
title: Add the voxel shade scale key
category: feature
release: 0.2.0
targets:
- type: key
  id: VoxelShadeScale
  effect: added
credit: [MentalHomiega]
---

`VoxelShadeScale=` in `[AudioVisual]` sets how many palette shade rows a lit voxel's brightness spans. The default, `16`, keeps the row a head-on surface takes at the neutral row. Values above 31 clamp to 31 rows, and a value of `0` or less shades every lit surface to row 0. The setting applies to bullets and voxel animations as well as units, aircraft and buildings.
