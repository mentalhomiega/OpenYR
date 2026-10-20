---
key: LowRadarBrightness
scope: theater
label: Theater radar floor
see_also: [HighRadarBrightness]
when_omitted:
  kind: context-dependent
  note: "`0.8` for SNOW, which keeps its original settings; `1.0` for every other theater."
---

`LowRadarBrightness` scales a tile's radar color. The radar multiplies each channel of the tile's low color by this value, rounds the result, and then halves it. At `1.0` the ground shows at half the brightness of its artwork's low color. Both halves of the cell take that color.

```ini title="rules.ini"
[DESERT]
LowRadarBrightness=1.0
```

Neither value applies to a cell whose radar color comes from what lies on it. A tree or other terrain object draws in a fixed light tan, and a bridge, Tiberium or other overlay shows its own color unscaled. A cell with no tile record draws gray.
