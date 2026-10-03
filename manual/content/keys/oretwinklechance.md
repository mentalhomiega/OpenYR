---
key: OreTwinkleChance
summary: "One in how many ore cells get a sparkle at the start of a game."
see_also: [OreTwinkle]
when_omitted:
  kind: value
  value: "0"
---

At the start of a scenario, each cell holding ore or gems gets the [`OreTwinkle`](/keys/oretwinkle/) animation with a chance of one in this many. With `0`, no cell gets one.

```ini title="rulesmd.ini"
[AudioVisual]
OreTwinkleChance=30
```
