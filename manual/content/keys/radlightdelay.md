---
key: RadLightDelay
summary: "How many frames apart a radiation patch's light dims."
see_also: [RadLightFactor, "system:radiation"]
when_omitted:
  kind: value
  value: "0"
---

A [radiation](/systems/radiation/#glow) patch's light dims one step every this many frames. A value of `0` counts as `1`.

```ini title="rulesmd.ini"
[Radiation]
RadLightDelay=90
```
