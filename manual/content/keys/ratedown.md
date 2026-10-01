---
key: RateDown
summary: "How much a gattling weapon's spin falls each frame it does not fire."
see_also: [RateUp, GuardAreaTargetingDelay, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

A [gattling](/systems/gattling-weapons/#when-the-spin-rises-and-falls) weapon's spin falls by this much each frame it is not firing, never below `0`. With `0`, the spin drops to `0` the first time it falls.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
RateDown=50
```
