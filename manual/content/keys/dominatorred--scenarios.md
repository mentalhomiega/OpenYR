---
key: DominatorRed
scope: scenarios
label: Scenario lighting
see_also: [DominatorGreen, DominatorBlue, Red, "system:superweapons"]
when_omitted:
  kind: value
  value: ".85"
---

```ini title="map file"
[Lighting]
DominatorRed=.85
```

While a [psychic dominator](/systems/superweapons/#psychic-dominator) blast lasts, `DominatorRed` is the multiplier on the red channel of every terrain palette and of the lit house color schemes. [`DominatorGreen`](/keys/dominatorgreen/) and [`DominatorBlue`](/keys/dominatorblue/) do the same for the green and blue channels. The scale matches the ordinary tint keys: `1` leaves colors unchanged, `0` removes the channel, and values are clamped to the range 0 through 2. The tint replaces the ordinary tint, and it changes in one step when the blast starts and again when it ends.
