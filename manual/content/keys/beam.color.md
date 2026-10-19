---
key: Beam.Color
summary: "The colour of the radiation beam of this weapon."
see_also: [IsRadBeam, RadColor, ChronoBeamColor]
when_omitted:
  kind: context-dependent
  note: "`ChronoBeamColor` for a weapon whose warhead is `Temporal=yes`, otherwise `RadColor`."
---

An [`IsRadBeam=yes`](/keys/isradbeam/) weapon draws its beam in this colour, written as red, green and blue from `0` to `255`. The brightness of the beam's wave is applied to it as it is to the default colours.

```ini title="rulesmd.ini"
[MyRadBeam] ; example WeaponType
IsRadBeam=yes
Beam.Color=255,100,0
```
