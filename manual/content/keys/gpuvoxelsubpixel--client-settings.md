---
key: GpuVoxelSubpixel
scope: client-settings
label: Smooth voxel motion when zoomed in
when_omitted:
  kind: value
  value: "no"
---

`GpuVoxelSubpixel=yes` places the moving objects that [`GpuVoxels`](/keys/gpuvoxels/) draws on the GPU by screen pixels instead of map pixels while the view is zoomed in. Without it a zoomed-in view shows a slow move, or the bob of a hover unit, in steps as large as one map pixel on screen; with it the same unit glides. With a `RenderFrameRate` above the game rate the position is also worked out for every drawn frame between two game frames. Only the drawing changes: the unit's place in the game, its cell and its selection box are the same.

The key is read from the `[Video]` section of `RA2MD.INI` at startup. The Graphics page of the settings sets it, and greys the switch until GPU drawing is on. It does nothing without `GpuVoxels=yes`, and nothing at a voxel scale of 1 (see [`GpuVoxelScale`](/keys/gpuvoxelscale/)), where a map pixel is the smallest step the picture has. Shadows and anything drawn from shapes stay on whole map pixels.
