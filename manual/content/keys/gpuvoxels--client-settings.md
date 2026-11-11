---
key: GpuVoxels
scope: client-settings
label: GPU voxel drawing
when_omitted:
  kind: value
  value: "no"
---

`GpuVoxels=yes` draws vehicles, aircraft, their shadows, voxel animations and voxel bullets from their voxels on the GPU, including vehicles with a voxel turret and barrel, instead of from the picture the software renders for them. The GPU draws each object straight into the map view's picture, as geometry with a depth buffer. The key is read from the `[Video]` section of `RA2MD.INI` at startup. The Graphics page of the settings sets it, and greys the switch until GPU drawing is on.

It takes effect only while `GpuRender=yes` draws the map view, in the modern interface on Direct3D 11. Vehicles with a shape body are still drawn by the software. A frame the GPU cannot draw is drawn by the software and shows the software's voxel pictures.

The picture differs from the software's in two ways. Where two voxels of one object cover the same pixel, the nearer one wins, on a turreted vehicle across hull, turret and barrel; [`GpuVoxelLayerDepth=Ordered`](/keys/gpuvoxellayerdepth/) keeps the software's drawing order. With `HDRHighlights=Enhanced` on an HDR display or with `Bloom=yes`, voxel objects show their highlights in the glow and the HDR picture, and a vehicle with a voxel turret shows them too, which the software does not give it. A cloaked voxel object is drawn plainly translucent, as in the software, unless [`GpuVoxelCloak=Ripple`](/keys/gpuvoxelcloak/) is set. [`GpuVoxelScale`](/keys/gpuvoxelscale/) and [`GpuVoxelSmooth`](/keys/gpuvoxelsmooth/) change the picture further.
