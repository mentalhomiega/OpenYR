---
key: RMGVegetationMinimums
summary: The lowest vegetation setting the random map dialog's randomize button picks, for each map type.
see_also: [RMGVegetationMaximums, RMGVegetationMapType, "system:map-generation"]
when_omitted:
  kind: value
  value: "60,60,60,60,60"
---

`RMGVegetationMinimums` and [`RMGVegetationMaximums`](/keys/rmgvegetationmaximums/) set the range from which the map generator dialog's randomize button picks [`Vegetation`](/keys/vegetation/). Each lists five whole numbers, one per Yuri's Revenge map type: archipelago, continent, team continent, inland and mountainous. This generator has no map types, so [`RMGVegetationMapType`](/keys/rmgvegetationmaptype/) picks which entry of each list is used.

```ini title="RMGMD.INI"
[General]
; archipelago, continent, team continent, inland, mountainous
RMGVegetationMinimums=60,60,60,60,60
RMGVegetationMaximums=100,100,100,100,100
```

Both entries are held to `0` to `100`, and a minimum above the maximum is lowered to it. The lists only affect the randomize button: a seed file, a `RandomMap=yes` scenario and the dialog's vegetation slider can still set any value.
