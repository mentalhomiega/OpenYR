---
key: WaterImage
summary: The vehicle type a vehicle of this type is drawn as while it is on water.
see_also: [UnloadingClass, Image]
when_omitted:
  kind: value
  value: "none"
---

`WaterImage` names another vehicle type. While a vehicle of this type is on a water cell and not on a bridge, it is drawn with that type's image, voxel or shape, and returns to its own on land. Only the drawing changes; the vehicle keeps its own type, weapons and locomotor. Name a type with the same kind of image, since a shape vehicle cannot be drawn with a voxel.

```ini title="rulesmd.ini"
[AMPHAPC] ; example VehicleType
WaterImage=AMPHAPCW
```
