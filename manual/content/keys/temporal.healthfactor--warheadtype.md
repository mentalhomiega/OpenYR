---
key: Temporal.HealthFactor
scope: warheadtype
summary: "How far the health an object has lost shortens the warp it takes to erase it."
see_also: [Temporal, Strength]
when_omitted:
  kind: value
  value: "0, so the warp does not depend on health"
---

A value from 0 to 1 for a [`Temporal=yes`](/keys/temporal/) warhead. The warp that erases an object starts at ten times a mix of the object's full `Strength=` and its current strength, and the value is the share taken from current strength: with `0` it is the full strength, with `1` the current strength, and a value between them lies between the two. A hurt object is therefore erased sooner. A value outside 0 to 1 is cut to the nearest end.

The warp is set when the first temporal weapon starts on the object. A weapon that joins the chain later does not change it.

```ini title="rulesmd.ini"
[MyChronoWH] ; example WarheadType
Temporal=yes
Temporal.HealthFactor=1.0
```
