---
key: RadApplicationDelay
summary: "How many frames apart radiation damages what stands in it."
see_also: [RadLevelFactor, "system:radiation"]
when_omitted:
  kind: value
  value: "0"
---

Every this many frames, objects on the ground take [radiation](/systems/radiation/#damage) damage from their cell. With `0`, radiation does no damage.

```ini title="rulesmd.ini"
[Radiation]
RadApplicationDelay=16
```
