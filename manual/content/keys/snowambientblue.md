---
key: SnowAmbientBlue
summary: The blue tint of a generated snow map, in hundredths, for each time of day.
see_also: [SnowAmbientLight, "system:map-generation"]
when_omitted:
  kind: value
  value: "133,107,154,118"
---

`SnowAmbientBlue` lists four whole numbers, for morning, day, dusk and night. On a tundra or taiga map, the map seed's [`Time`](/keys/time/) picks one, and the map plays with it as its blue tint. `100` leaves blue unchanged, a lower entry weakens it and a higher one strengthens it.

```ini title="RMGMD.INI"
[General]
; morning, day, dusk, night
SnowAmbientBlue=133,107,154,118
```

[`SnowAmbientLight`](/keys/snowambientlight/) sets the same map's ambient light. [RMGMD.INI](/formats/rmgmd-ini/) explains how a short list is read.
