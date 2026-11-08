---
key: DominatorAmbientChangeRate
scope: scenarios
label: Scenario lighting
see_also: [DominatorAmbient, AmbientChangeStep, "system:superweapons"]
when_omitted:
  kind: value
  value: ".001"
---

```ini title="map file"
[Lighting]
DominatorAmbientChangeRate=.001
```

The time between one step of the ambient light and the next while a [psychic dominator](/systems/superweapons/#psychic-dominator) blast lasts, including the settling back that follows it, in thousands of game frames. `.001` is one frame and `1` is 1,000 frames. The value is kept in whole frames, and any finer part is dropped. The size of each step is [`AmbientChangeStep`](/keys/ambientchangestep/), and the fade does not run at all while [`AmbientChangeRate`](/keys/ambientchangerate/) is `0`.
