---
key: OccupyHeight
scope: buildingtype
label: 'Bunkered vehicle draw height'
see_also: [Bunker, CanHideThings, "system:tank-bunkers"]
when_omitted:
  kind: value
  value: "0"
---

A vehicle in this [`Bunker=yes`](/keys/bunker/#scope-buildingtype) structure is drawn in front of it as if it stood this many height levels higher, so less of it is hidden behind the bunker's floor. Its position and height in the game do not change. Set the key in the structure's art section.

For a [`CanHideThings=yes`](/keys/canhidethings/#scope-buildingtype) structure, this key also sets how far back its cover reaches: each foundation cell and the cells behind it, `OccupyHeight` minus one in all and at least one, are covered.

```ini title="artmd.ini"
[MYBUNKER] ; example art section
OccupyHeight=2
```
