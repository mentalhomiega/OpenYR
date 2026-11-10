---
key: SteadyVoxelPlacement
scope: client-settings
label: Steady voxel placement
when_omitted:
  kind: value
  value: "yes"
---

`SteadyVoxelPlacement=yes` places each voxel picture, such as a vehicle or aircraft and its shadow, on a whole pixel. A unit that turns then stays in place, where the original game's placement moves it by up to a pixel from one facing to the next. `SteadyVoxelPlacement=no` places pictures as the original game does.

The key is read from the `[Video]` section of `RA2MD.INI` and applies to both the software drawing and [`GpuVoxels`](/keys/gpuvoxels/). The Graphics page of the settings sets it, and a change takes effect on the next frame. Only drawing changes: the game logic is the same.
