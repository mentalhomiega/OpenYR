---
key: DisableVoxelCache
scope: aircrafttype
label: Voxel body cache
see_also: [DisableShadowCache, NoShadow]
when_omitted:
  kind: value
  value: "no"
---

A voxel object normally draws its body from an image it rendered earlier for its current facing. `DisableVoxelCache=yes` in the type's art section makes every object of the type render its body again each frame, which costs more time per object, so set it only on the types that need it. The key is read from the art section named by the type's [`Image`](/keys/image/). A type with no voxel body is unaffected.
