---
key: OccupyHeight
scope: buildingtype
label: 'Covered cells behind the structure'
see_also: [CanHideThings, "system:tank-bunkers"]
when_omitted:
  kind: value
  value: "0"
---

The height of the structure in height levels. Set the key in the structure's art section. It does not change how a vehicle in a [`Bunker=yes`](/keys/bunker/#scope-buildingtype) structure is drawn: the bunker's wall animations, their `ZAdjust` and `YSort`, put the walls over the vehicle's lower hull.

For a [`CanHideThings=yes`](/keys/canhidethings/#scope-buildingtype) structure, this key sets how far back its cover reaches: each foundation cell and the cells behind it, `OccupyHeight` minus one in all and at least one, are covered.

```ini title="artmd.ini"
[MYBUNKER] ; example art section
OccupyHeight=2
```
