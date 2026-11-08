---
key: TemperateAmbientRed
summary: The red tint of a generated temperate map, in hundredths, for each time of day.
see_also: [TemperateAmbientLight, "system:map-generation"]
when_omitted:
  kind: value
  value: "109,108,71,51"
---

`TemperateAmbientRed` lists four whole numbers, for morning, day, dusk and night. On a temperate, desert or mutated map, the map seed's [`Time`](/keys/time/) picks one, and the map plays with it as its red tint. `100` leaves red unchanged, a lower entry weakens it and a higher one strengthens it.

```ini title="RMGMD.INI"
[General]
; morning, day, dusk, night
TemperateAmbientRed=109,108,71,51
```

[`TemperateAmbientLight`](/keys/temperateambientlight/) sets the same map's ambient light. [RMGMD.INI](/formats/rmgmd-ini/) explains how a short list is read.
