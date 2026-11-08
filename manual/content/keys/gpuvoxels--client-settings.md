---
key: GpuVoxels
scope: client-settings
label: GPU voxel drawing
when_omitted:
  kind: value
  value: "no"
---

`GpuVoxels=yes` draws vehicles and aircraft that have no turret from their voxels on the GPU, instead of from the picture the software renders for them. The picture is the same. The key is read from the `[Video]` section of `RA2MD.INI` at startup, and the settings pages have no switch for it.

It takes effect only while `GpuRender=yes` draws the map view, in the modern interface on Direct3D 11. Turreted vehicles, shadows, voxel animations and bullets are still drawn by the software. While the emissive mask is on, with `HDRHighlights=Enhanced` on an HDR display or with `Bloom=yes`, nothing is drawn this way.
