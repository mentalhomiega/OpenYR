---
key: GpuVoxelSmooth
scope: client-settings
label: Smooth voxel facing
when_omitted:
  kind: value
  value: "no"
---

`GpuVoxelSmooth=yes` turns the vehicles and aircraft that [`GpuVoxels`](/keys/gpuvoxels/) draws on the GPU, and their shadows, to their exact facing, instead of to the nearest of the 32 facings the software draws. A unit that turns is drawn at every angle on the way, and with a `RenderFrameRate` above the game rate it turns smoothly between game frames, the short way round. Only the drawing changes: the facing the game logic uses is the same.

The key is read from the `[Video]` section of `RA2MD.INI` at startup. The Graphics page of the settings sets it, and greys the switch until GPU drawing is on. It does nothing without `GpuVoxels=yes`. Shadows follow the exact facing of their object, and a turret turns to its own exact facing; the barrel's pitch stays in steps. At each of the 32 facings the picture is the same as with the key off.
