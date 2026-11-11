---
key: GpuVoxelScale
scope: client-settings
label: GPU voxel sharpness
when_omitted:
  kind: value
  value: "0"
---

`GpuVoxelScale=` sets how many times finer than the map's own pixels the GPU draws the map view, from 1 to 4. At 2 to 4, each voxel that [`GpuVoxels`](/keys/gpuvoxels/) draws is its cube instead of a one or two pixel square, so a unit stays sharp when the map view is zoomed in. `GpuVoxelScale=0`, the default, takes the value from the zoom: the number of screen pixels one map pixel covers, rounded to the nearest whole number and held to 1 to 4. A map view shown at less than 1.5 screen pixels a map pixel is then drawn at 1. The key is read from the `[Video]` section of `RA2MD.INI` at startup. The Graphics page of the settings sets it with a slider, which shows 0 as **Auto**, and greys the slider until GPU drawing is on.

It takes effect only while `GpuVoxels=yes` and `GpuRender=yes` draw the map view. A larger value costs video memory and drawing time for the whole map view. A map view too large to draw at the value is drawn at the largest value that fits, down to 1.
