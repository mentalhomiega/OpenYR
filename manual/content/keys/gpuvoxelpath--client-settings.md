---
key: GpuVoxelPath
scope: client-settings
label: GPU voxel route
when_omitted:
  kind: value
  value: Image
---

`GpuVoxelPath=Direct` draws the voxel objects that [`GpuVoxels`](/keys/gpuvoxels/) puts on the GPU straight into the map view's picture, as geometry with a depth buffer, instead of first drawing each one into a shared voxel image. `Image`, the default, keeps the voxel image. An unknown value reads as `Image`. The key is read from the `[Video]` section of `RA2MD.INI`; the settings pages have no row for it.

With `Direct`, an object takes the direct route when it is one voxel object without a turret, such as a hull-only vehicle or an aircraft, and it is drawn without translucency. Turreted vehicles, voxel shadows and cloaked or warping objects still take the voxel image. While Glow or HDR Enhanced highlights are on, every voxel object takes the voxel image. A direct object has no picture to overflow, so many large objects at a high [`GpuVoxelScale`](/keys/gpuvoxelscale/) no longer push the map view back to software drawing.

[`GpuVoxelLayerDepth`](/keys/gpuvoxellayerdepth/) sets how the parts of one direct object hide each other.
