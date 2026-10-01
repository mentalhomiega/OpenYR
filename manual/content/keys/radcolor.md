---
key: RadColor
summary: "The color of the beam an IsRadBeam weapon draws."
see_also: [IsRadBeam, ChronoBeamColor, RadTintFactor, "system:radiation"]
when_omitted:
  kind: value
  value: "0,0,0"
---

An [`IsRadBeam=yes`](/keys/isradbeam/) weapon draws its beam in this color, as red, green and blue values from `0` to `255`, unless its warhead is temporal.

```ini title="rulesmd.ini"
[Radiation]
RadColor=0,255,0
```

It also tints the glow of [radiation](/systems/radiation/#glow) patches.
