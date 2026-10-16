---
title: Keep a voxel shade row inside the palette when VoxelShadeScale is not a number
category: fix
release: 0.2.0
targets:
- type: key
  id: VoxelShadeScale
  effect: changed
credit: [MentalHomiega]
---

`VoxelShadeScale=nan` left the shade row of a voxel surface undefined, and the voxel drawers index the palette lookup with that row unchecked. A value that is not a number now shades every lit surface to row 0, as a value of `0` does.
