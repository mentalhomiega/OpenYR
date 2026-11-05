---
key: RMGVegetationMaximums
summary: The highest vegetation setting the random map dialog's randomize button picks, for each map type.
see_also: [RMGVegetationMinimums, RMGVegetationMapType, "system:map-generation"]
when_omitted:
  kind: value
  value: "100,100,100,100,100"
---

`RMGVegetationMaximums` lists the highest [`Vegetation`](/keys/vegetation/) setting the map generator dialog's randomize button picks, one entry per map type. [`RMGVegetationMinimums`](/keys/rmgvegetationminimums/) gives the lowest and explains how the two lists are used.

```ini title="RMGMD.INI"
[General]
RMGVegetationMaximums=100,100,100,100,100
```
