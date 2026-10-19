---
key: DisableShadowCache
scope: aircrafttype
label: Voxel shadow cache
see_also: [DisableVoxelCache, NoShadow]
when_omitted:
  kind: value
  value: "no"
---

A voxel object normally draws its shadow from an image it rendered earlier for its current facing. `DisableShadowCache=yes` in the type's art section makes every object of the type render its shadow again each frame. The key is read from the art section named by the type's [`Image`](/keys/image/), and has no effect on a type with [`NoShadow=yes`](/keys/noshadow/) or with no voxel body.
