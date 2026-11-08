---
key: TemperateAmbientBlue
summary: The blue tint of a generated temperate map, in hundredths, for each time of day.
see_also: [TemperateAmbientLight, "system:map-generation"]
when_omitted:
  kind: value
  value: "49,68,119,141"
---

`TemperateAmbientBlue` lists four whole numbers, for morning, day, dusk and night. On a temperate, desert or mutated map, the map seed's [`Time`](/keys/time/) picks one, and the map plays with it as its blue tint. `100` leaves blue unchanged, a lower entry weakens it and a higher one strengthens it.

```ini title="RMGMD.INI"
[General]
; morning, day, dusk, night
TemperateAmbientBlue=49,68,119,141
```

[`TemperateAmbientLight`](/keys/temperateambientlight/) sets the same map's ambient light. [RMGMD.INI](/formats/rmgmd-ini/) explains how a short list is read.
