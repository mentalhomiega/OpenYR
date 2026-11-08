---
key: TemperateAmbientLight
summary: The ambient light of a generated temperate map, in hundredths of full daylight, for each time of day.
see_also: [SnowAmbientLight, RMGLevelLightSettings, "system:map-generation"]
when_omitted:
  kind: value
  value: "75,100,75,35"
---

`TemperateAmbientLight` lists four whole numbers, for morning, day, dusk and night. On a temperate, desert or mutated map, the map seed's [`Time`](/keys/time/) picks one, and the map plays with it as its ambient light, where `100` is full daylight. The map returns to the same level when an ion storm ends. The generator also writes it as the map's [`IonAmbient`](/keys/ionambient/#scope-random-map-generation), which a `UseIonStorms=yes` seed's `ION.INI` can replace.

```ini title="RMGMD.INI"
[General]
; morning, day, dusk, night
TemperateAmbientLight=75,100,75,35
```

[`SnowAmbientLight`](/keys/snowambientlight/) lights the other theater. [RMGMD.INI](/formats/rmgmd-ini/) explains how a short list is read.
