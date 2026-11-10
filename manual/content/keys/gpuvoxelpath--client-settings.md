---
key: GpuVoxelPath
scope: client-settings
label: GPU voxel route
when_omitted:
  kind: value
  value: Image
---

`GpuVoxelPath=Direct` draws the voxel objects that [`GpuVoxels`](/keys/gpuvoxels/) puts on the GPU straight into the map view's picture, as geometry with a depth buffer, instead of first drawing each one into a shared voxel image. `Image`, the default, keeps the voxel image. An unknown value reads as `Image`. The key is read from the `[Video]` section of `RA2MD.INI`; the settings pages have no row for it.

With `Direct`, voxel objects take the direct route: hull-only vehicles, aircraft, other plain voxel objects and vehicles with a voxel turret and barrel, drawn solid or translucent, as a warping object is. The shadows of voxel objects take it too, with or without `SoftShadows`. A cloaked object is drawn plainly translucent on both routes, without the ripple of the software renderer. While Glow or HDR Enhanced highlights are on, a direct object shows its highlights as it does through the voxel image, and a vehicle with a voxel turret gets highlights too, which the voxel image and the software renderer do not give it. A direct object takes no room in the voxel image, so it keeps the full [`GpuVoxelScale`](/keys/gpuvoxelscale/) when many large objects would make the voxel image draw some of its objects at a lower scale.

[`GpuVoxelLayerDepth`](/keys/gpuvoxellayerdepth/) sets how the parts of one direct object hide each other.
