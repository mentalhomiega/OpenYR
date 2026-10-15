---
key: VoxelSpecularStrength
scope: global-rules
label: Voxel specular strength
summary: Sets the strength of the highlight that units, aircraft and buildings drawn as voxels take from the viewer's direction.
when_omitted:
  kind: value
  value: "3"
---

A voxel drawn for a unit, an aircraft or a building takes a highlight on top of its diffuse shading. The highlight for each surface depends on how closely the halfway direction between the light and the viewer lines up with that surface's normal. This key sets the strength `s` in the highlight `h / (s - h * s + h)`, where `h` is that alignment. At `1` the highlight rises linearly with the alignment. Higher values concentrate it on surfaces nearly aligned with the halfway direction. A surface turned away from the halfway direction (`h` of `0` or less) takes no highlight at any strength.

A value of `0` or less turns the highlight off, and those voxels are shaded from the diffuse term alone. Bullets and voxel animations never take the highlight. [`VoxelShadeScale`](/keys/voxelshadescale/) scales the brightness the highlight adds.

Set it in the `[AudioVisual]` section.

```ini title="rules.ini"
[AudioVisual]
VoxelSpecularStrength=3
```
