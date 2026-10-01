---
key: RadLevelMax
summary: "The most radiation a cell counts when it does damage."
see_also: [RadLevelFactor, "system:radiation"]
when_omitted:
  kind: value
  value: "0"
---

A cell's radiation counts up to this value when it [damages](/systems/radiation/#damage) what stands in it, whatever its patches add up to. With `0`, radiation does no damage.

```ini title="rulesmd.ini"
[Radiation]
RadLevelMax=500
```
