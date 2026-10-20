---
key: HighRadarBrightness
scope: theater
label: Theater radar ceiling
see_also: [LowRadarBrightness]
when_omitted:
  kind: context-dependent
  note: "`1.1` for SNOW, which keeps its original settings; `1.6` for every other theater."
---

`HighRadarBrightness` has no effect on the radar. The engine reads the value, but the radar draws a tile's color the same at every height, so raised ground does not show brighter than low ground.

Use [`LowRadarBrightness`](/keys/lowradarbrightness/) to change how dark or light a theater's ground shows on the radar.
