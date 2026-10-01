---
key: GuardAreaTargetingDelay
summary: "How long a gattling structure waits after its last shot before its spin starts to fall every frame."
see_also: [RateDown, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "36"
---

Out of its attack mission, a [gattling](/systems/gattling-weapons/#when-the-spin-rises-and-falls) structure's spin falls by its `RateDown` every frame once this many frames, plus five, have passed since its last shot.

```ini title="rulesmd.ini"
[General]
GuardAreaTargetingDelay=36
```
