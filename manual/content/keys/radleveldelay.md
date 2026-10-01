---
key: RadLevelDelay
summary: "How many frames apart a radiation patch steps down."
see_also: [RadDurationMultiple, "system:radiation"]
when_omitted:
  kind: value
  value: "0"
---

Every this many frames, each cell of a [radiation](/systems/radiation/#fading) patch loses an equal step of what the patch gave it. A value of `0` counts as `1`.

```ini title="rulesmd.ini"
[Radiation]
RadLevelDelay=90
```
