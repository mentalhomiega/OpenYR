---
key: Palette
scope: aircrafttype
summary: "The palette file this object type is drawn with."
see_also: [CustomPalette, TerrainPalette, Remapable]
when_omitted:
  kind: value
  value: "none, so the object uses the palette of its house colour"
---

The value is a palette file name without `.pal`. The game reads that name followed by the theater's three letters, such as `holotem.pal`, and falls back to `holo.pal` when that file is missing. The object's colours come from the file, with the owner's colour in the sixteen remap entries. The map's lighting applies to it as to any palette, and an aircraft takes no tint from the Iron Curtain or a berserk state. The animations of a structure with the key are drawn through the palette as well.

```ini title="artmd.ini"
[CAHOLO01] ; example BuildingType
Palette=holo
```
