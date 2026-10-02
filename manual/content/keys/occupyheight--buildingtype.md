---
key: OccupyHeight
scope: buildingtype
label: 'Bunkered vehicle draw height'
see_also: [Bunker, "system:tank-bunkers"]
when_omitted:
  kind: value
  value: "0"
---

A vehicle in this [`Bunker=yes`](/keys/bunker/#scope-buildingtype) structure is drawn in front of it as if it stood this many height levels higher, so less of it is hidden behind the bunker's floor. Its position and height in the game do not change. Set the key in the structure's art section.

```ini title="artmd.ini"
[MYBUNKER] ; example art section
OccupyHeight=2
```
