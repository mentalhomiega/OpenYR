---
key: RMGLevelLightSettings
summary: How much light each height level adds on a generated map, in thousandths of full light, for each time of day.
see_also: [TemperateAmbientLight, SnowAmbientLight, "system:map-generation"]
when_omitted:
  kind: value
  value: "3,3,3,3"
---

`RMGLevelLightSettings` lists four whole numbers, for morning, day, dusk and night. A map seed's [`Time`](/keys/time/) picks one, and each height level a cell stands above the ground adds that many thousandths of full light. The stock `3` works as [`Level=.003`](/keys/level/#scope-scenarios-2) does in a map file. The same list applies on every biome.

```ini title="RMGMD.INI"
[General]
; morning, day, dusk, night
RMGLevelLightSettings=3,3,3,3
```

[RMGMD.INI](/formats/rmgmd-ini/) explains how a short list is read.
