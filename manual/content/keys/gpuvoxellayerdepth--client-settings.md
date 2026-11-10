---
key: GpuVoxelLayerDepth
scope: client-settings
label: GPU voxel layer depth
when_omitted:
  kind: value
  value: Shared
---

`GpuVoxelLayerDepth=` sets how the voxel layers of one object hide each other when [`GpuVoxelPath=Direct`](/keys/gpuvoxelpath/) draws it. `Shared`, the default, lets the nearer voxel win wherever two voxels of the object cover the same pixel. `Ordered` keeps the software's drawing order, so the later voxel always wins and the picture is the same as the voxel image route's. An unknown value reads as `Shared`.

The key is read from the `[Video]` section of `RA2MD.INI`; the settings pages have no row for it. It changes nothing for objects that take the voxel image, and objects never hide each other by it: a later object is always drawn over an earlier one, as in the software.
