---
key: DominatorAmbientChangeRate
scope: scenarios
label: Scenario lighting
see_also: [DominatorAmbient, AmbientChangeStep, "system:superweapons"]
when_omitted:
  kind: value
  value: "1"
---

```ini title="map file"
[Lighting]
DominatorAmbientChangeRate=1
```

The number of game frames between one step of the ambient light and the next while a [psychic dominator](/systems/superweapons/#psychic-dominator) blast lasts, including the settling back that follows it. A fraction is dropped. The size of each step is [`AmbientChangeStep`](/keys/ambientchangestep/), and the fade does not run at all while [`AmbientChangeRate`](/keys/ambientchangerate/) is `0`.
