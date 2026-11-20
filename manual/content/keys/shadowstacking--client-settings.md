---
key: ShadowStacking
scope: client-settings
label: Stacked shadows
when_omitted:
  kind: value
  value: "yes"
---

`ShadowStacking=yes` darkens the ground again under each shadow that overlaps another, as the original game does: eight Kirovs over one spot leave the ground nearly black. `ShadowStacking=no` darkens it once, so overlapping vehicle and aircraft shadows leave a grey patch the depth of a single shadow, with hard edges.

The key is read from the `[Video]` section of `RA2MD.INI`. The Graphics page sets it with the **Darker overlapping shadows** switch, and a change takes effect on the next frame. It applies to software drawing and to every GPU path, with [`GpuVoxels`](/keys/gpuvoxels/) on or off. With `no`, the shadows of voxel objects darken the ground once in software; with GPU drawing on, every shadow drawn in the map pass joins the same single darkening, shape shadows of infantry and buildings included. Shadows that buildings and trees draw into the terrain layer are not changed. Only drawing changes: the game logic is the same.
