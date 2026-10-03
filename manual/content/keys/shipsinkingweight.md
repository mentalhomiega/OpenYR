---
key: ShipSinkingWeight
summary: "Weight at or above which a destroyed ship sinks instead of vanishing."
see_also: [Weight, SinkingSound]
when_omitted:
  kind: value
  value: "3"
---

A destroyed vehicle sinks to the bottom when all of these hold: its type is `Naval=yes`, neither `Underwater=yes` nor `Organic=yes`, and its [`Weight`](/keys/weight/) is at least this value, and it is on open water rather than a bridge or beach. The wreck plays the [sinking sound](/keys/sinkingsound/#scope-aircrafttype), tilts forward and slips under the surface, and is removed once it is deep enough. It takes no further damage on the way down and is counted as lost when it is destroyed.

```ini title="rulesmd.ini"
[General]
ShipSinkingWeight=3.0
```
