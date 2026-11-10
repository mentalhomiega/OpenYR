---
key: GpuVoxelPath
scope: client-settings
label: GPU voxel route
when_omitted:
  kind: value
  value: Image
---

`GpuVoxelPath=Direct` draws the voxel objects that [`GpuVoxels`](/keys/gpuvoxels/) puts on the GPU straight into the map view's picture, as geometry with a depth buffer, instead of first drawing each one into a shared voxel image. `Image`, the default, keeps the voxel image. An unknown value reads as `Image`. The key is read from the `[Video]` section of `RA2MD.INI`; the settings pages have no row for it.

With `Direct`, an object takes the direct route when it is drawn without translucency: a hull-only vehicle, an aircraft, another plain voxel object, or a vehicle with a voxel turret and barrel. The shadows of such objects take it too while `SoftShadows` is off. Cloaked or warping objects and every shadow under soft shadows still take the voxel image. While Glow or HDR Enhanced highlights are on, every voxel object takes the voxel image. A direct object takes no room in the voxel image, so it keeps the full [`GpuVoxelScale`](/keys/gpuvoxelscale/) when many large objects would make the voxel image draw some of its objects at a lower scale.

[`GpuVoxelLayerDepth`](/keys/gpuvoxellayerdepth/) sets how the parts of one direct object hide each other.
