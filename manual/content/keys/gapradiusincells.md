---
key: GapRadiusInCells
summary: "How far a gap generator shrouds the ground around it, in cells."
see_also: [GapGenerator, "system:map-visibility"]
when_omitted:
  kind: value
  value: "0"
---

A [`GapGenerator=yes`](/keys/gapgenerator/) structure of this type [covers](/systems/map-visibility/#gap-generators) every cell within this many cells of the cell it is drawn over. With `0`, it covers only that cell.

```ini title="rulesmd.ini"
[MYGAP] ; example BuildingType
GapRadiusInCells=10
```
