---
key: OreTwinkle
summary: "The sparkle animation placed on some ore cells at the start of a game."
see_also: [OreTwinkleChance]
when_omitted:
  kind: value
  value: none
---

When a scenario starts, each cell holding ore or gems gets this animation with a chance of one in [`OreTwinkleChance`](/keys/oretwinklechance/). The animation is placed once and plays by its own art settings; ore that grows later gets none.

```ini title="rulesmd.ini"
[General]
OreTwinkle=TWNK1
```
