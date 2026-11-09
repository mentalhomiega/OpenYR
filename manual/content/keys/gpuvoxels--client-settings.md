---
key: GpuVoxels
scope: client-settings
label: GPU voxel drawing
when_omitted:
  kind: value
  value: "no"
---

`GpuVoxels=yes` draws vehicles and aircraft from their voxels on the GPU, including vehicles with a voxel turret and barrel, instead of from the picture the software renders for them. The picture is the same. The key is read from the `[Video]` section of `RA2MD.INI` at startup, and the settings pages have no switch for it.

It takes effect only while `GpuRender=yes` draws the map view, in the modern interface on Direct3D 11. Shadows, voxel animations, bullets and vehicles with a shape body are still drawn by the software. With `HDRHighlights=Enhanced` on an HDR display or with `Bloom=yes`, vehicles and aircraft that have no turret keep their voxel highlights in the glow and the HDR picture, and a vehicle with a voxel turret has none, as in the software picture.
