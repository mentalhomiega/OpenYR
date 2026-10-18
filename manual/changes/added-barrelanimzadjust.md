---
title: Add the voxel barrel depth bias key
category: feature
release: 0.2.0
targets:
- type: key
  id: BarrelAnimZAdjust
  effect: added
credit:
- MentalHomiega
---

`BarrelAnimZAdjust=` in a building's section sets the depth bias, in pixels, of the voxel barrel drawn for `BarrelAnimIsVoxel=yes`. A lower value draws the barrel further in front. The default, `-21`, draws the barrel as it was before, so a building that does not set the key looks the same.
