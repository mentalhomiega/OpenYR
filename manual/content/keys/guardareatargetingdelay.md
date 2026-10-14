---
key: GuardAreaTargetingDelay
summary: "Frames an area-guarding object waits between target scans, and how long a gattling structure waits before its spin falls."
see_also: [RateDown, "system:gattling-weapons", "system:target-selection"]
when_omitted:
  kind: value
  value: "36"
---

Each area scan restarts the object's [targeting timer](/systems/target-selection/#the-targeting-timer) with this many frames, plus a random 0 to 2. The next area scan waits until the timer runs out.

Out of its attack mission, a [gattling](/systems/gattling-weapons/#when-the-spin-rises-and-falls) structure's spin falls by its `RateDown` every frame once this many frames, plus five, have passed since its last shot.

```ini title="rulesmd.ini"
[General]
GuardAreaTargetingDelay=36
```
