---
key: VoxelShadeScale
scope: global-rules
label: Voxel shade scale
summary: Sets how many palette shade rows a voxel's diffuse and highlight brightness spans.
when_omitted:
  kind: value
  value: "16"
---

A lit voxel's shade row is its brightness multiplied by this value, truncated, and clamped to rows 0 through 31 of the voxel palette library. Brightness is the diffuse term, which is `1` for a surface the light meets head-on, plus the highlight from [`VoxelSpecularStrength`](/keys/voxelspecularstrength/). At the default of `16`, a head-on surface takes row 16, which is the neutral row, and only a highlight takes a surface above it.

Rows above 31 clamp to 31. A value of `0` or less shades every lit surface to row 0. The setting applies to every voxel drawn with lighting: units, aircraft, buildings, bullets and voxel animations.

Set it in the `[AudioVisual]` section.

```ini title="rules.ini"
[AudioVisual]
VoxelShadeScale=16
```
