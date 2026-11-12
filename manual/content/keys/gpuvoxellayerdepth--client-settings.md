---
key: GpuVoxelLayerDepth
scope: client-settings
label: GPU voxel layer depth
when_omitted:
  kind: value
  value: Shared
---

`GpuVoxelLayerDepth=` sets how the voxel layers of one object hide each other when [`GpuVoxels=yes`](/keys/gpuvoxels/) draws it. `Shared`, the default, lets the nearer voxel win wherever two voxels of the object cover the same pixel; on a turreted vehicle this includes the hull, turret and barrel. `Ordered` keeps the software's drawing order, so the later voxel always wins, as in the software. An unknown value reads as `Shared`.

The key is read from the `[Video]` section of `RA2MD.INI`; the settings pages have no row for it. Objects never hide each other by it: a later object is always drawn over an earlier one, as in the software.
