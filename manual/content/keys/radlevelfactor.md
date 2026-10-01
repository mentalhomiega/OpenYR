---
key: RadLevelFactor
summary: "The damage each point of a cell's radiation does."
see_also: [RadApplicationDelay, RadLevelMax, "system:radiation"]
when_omitted:
  kind: value
  value: "0.0"
---

A cell's radiation, up to `RadLevelMax`, times this value, rounded down, is the [damage](/systems/radiation/#damage) it does each time radiation damage applies.

```ini title="rulesmd.ini"
[Radiation]
RadLevelFactor=0.2
```
