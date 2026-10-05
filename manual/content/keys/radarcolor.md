---
key: RadarColor
summary: The color a terrain object paints on the radar and on the multiplayer map preview.
when_omitted:
  kind: computed
  note: The first color of the type's shape artwork, or black when the type has no shape loaded.
---

A cell that holds a terrain object, such as a tree, is drawn on the radar in the object type's `RadarColor`. Write the value as three numbers for red, green and blue, each from 0 to 255. A value that is not three numbers leaves the artwork color in place.

```ini title="rulesmd.ini"
[TREE01] ; example TerrainType
RadarColor=20,90,20
```
