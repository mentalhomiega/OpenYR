---
key: VoxelSpecularFromView
scope: global-rules
label: Voxel specular from view
summary: Takes the viewer direction for the voxel highlight from the isometric view alone, not from the voxel's own transform.
when_omitted:
  kind: value
  value: "no"
---

The highlight needs the direction the viewer sees a voxel from, expressed in the voxel's own frame. By default that direction comes from the transform the voxel is drawn with, so it turns as the unit turns. At `yes`, it comes from the isometric view transform alone, so the viewer direction stays fixed while the unit turns. The light direction is the same in both settings.

This key has no effect while [`VoxelSpecularStrength`](/keys/voxelspecularstrength/) is `0` or less.

Set it in the `[AudioVisual]` section.

```ini title="rules.ini"
[AudioVisual]
VoxelSpecularFromView=no
```
