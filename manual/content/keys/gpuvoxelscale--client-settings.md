---
key: GpuVoxelScale
scope: client-settings
label: GPU voxel sharpness
when_omitted:
  kind: value
  value: "1"
---

`GpuVoxelScale=` sets how many times finer than the map's own pixels the GPU draws the map view, from 1 to 4. At 2 to 4, each voxel that [`GpuVoxels`](/keys/gpuvoxels/) draws is the outline of its cube instead of a one or two pixel square, so a unit stays sharp when the map view is zoomed in. The key is read from the `[Video]` section of `RA2MD.INI` at startup. The Graphics page of the settings sets it with a slider, and greys the slider until GPU drawing is on.

It takes effect only while `GpuVoxels=yes` and `GpuRender=yes` draw the map view. At 1 the picture is the same as without the key. A larger value costs video memory and drawing time for the whole map view, and a map view too large to draw at the scale is drawn by the software instead.
