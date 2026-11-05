---
key: RMGVegetationMapType
summary: Which map type entry of the vegetation lists the random map dialog uses, from 0 to 4.
see_also: [RMGVegetationMinimums, RMGVegetationMaximums, "system:map-generation"]
when_omitted:
  kind: value
  value: "1"
  note: The continent entries are used, as on the Yuri's Revenge dialog's default map type.
---

`RMGVegetationMapType` picks the entry of [`RMGVegetationMinimums`](/keys/rmgvegetationminimums/) and [`RMGVegetationMaximums`](/keys/rmgvegetationmaximums/) that the map generator dialog's randomize button uses. The lists hold one entry per Yuri's Revenge map type, and this generator has no map types:

| Value | Map type |
| --- | --- |
| `0` | Archipelago |
| `1` | Continent |
| `2` | Team continent |
| `3` | Inland |
| `4` | Mountainous |

```ini title="RMGMD.INI"
[General]
RMGVegetationMapType=3
```

A value outside `0` to `4` is ignored, and the continent entries are used.
