---
title: Give no voxel highlight to surfaces turned away from the light
category: fix
release: 0.2.0
targets:
- type: key
  id: VoxelSpecularStrength
  effect: changed
credit: [MentalHomiega]
---

With `VoxelSpecularStrength=` between `0` and `0.5`, the surfaces of a voxel turned well away from the viewer and the light took a bright highlight. A surface turned away from the halfway direction of the light and the viewer now takes none. The default of `3` and every value from `0.5` up draw as before.
