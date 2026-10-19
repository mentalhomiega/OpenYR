---
key: Palette
scope: terraintype
summary: "The palette file a terrain object of this type is drawn with."
see_also: [TerrainPalette, CustomPalette]
when_omitted:
  kind: value
  value: "the palette of the tile the object stands on"
---

The value is a palette file name without `.pal` and without the theater's suffix, which the game adds. The object takes its colours from the file and ignores the tint of the tile under it.

```ini title="artmd.ini"
[TREE01] ; example TerrainType
Palette=lib
```
