---
key: ChronoBeamColor
summary: "The color of the beam an IsRadBeam weapon with a temporal warhead draws."
see_also: [IsRadBeam, RadColor, "system:temporal-weapons"]
when_omitted:
  kind: value
  value: "0,0,0"
---

An [`IsRadBeam=yes`](/keys/isradbeam/) weapon whose warhead is [`Temporal=yes`](/keys/temporal/) draws its beam in this color, as red, green and blue values from `0` to `255`.

```ini title="rulesmd.ini"
[AudioVisual]
ChronoBeamColor=128,200,255
```
