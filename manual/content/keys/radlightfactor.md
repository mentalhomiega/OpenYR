---
key: RadLightFactor
summary: "How bright a radiation patch's light is for each point of its level."
see_also: [RadTintFactor, "system:radiation"]
when_omitted:
  kind: value
  value: "0.0"
---

A [radiation](/systems/radiation/#glow) patch's light is as bright as its level times this value, up to `2000`.

```ini title="rulesmd.ini"
[Radiation]
RadLightFactor=0.1
```
