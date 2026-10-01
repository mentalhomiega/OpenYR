---
key: RadDurationMultiple
summary: "How many frames a radiation patch lasts for each point of its level."
see_also: [RadLevel, RadLevelDelay, "system:radiation"]
when_omitted:
  kind: value
  value: "0"
---

A [radiation](/systems/radiation/#fading) patch lasts its level times this many frames.

```ini title="rulesmd.ini"
[Radiation]
RadDurationMultiple=1
```
