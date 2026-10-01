---
key: RateUp
summary: "How much a gattling weapon's spin rises each frame it fires."
see_also: [RateDown, Stage1, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

A [gattling](/systems/gattling-weapons/#when-the-spin-rises-and-falls) weapon's spin rises by this much each frame it fires, faces its target or reloads, while the spin is below its last stage's threshold.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
RateUp=1
```
