---
key: RadTintFactor
summary: "How strongly a radiation patch's light takes RadColor."
see_also: [RadColor, RadLightFactor, "system:radiation"]
when_omitted:
  kind: value
  value: "0.0"
---

A [radiation](/systems/radiation/#glow) patch's light is tinted by `RadColor`, scaled so `255` means full strength, times this value, each part capped at `2000`.

```ini title="rulesmd.ini"
[Radiation]
RadTintFactor=1.0
```
