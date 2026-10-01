---
key: GapGenerator
summary: "Makes a structure shroud the ground around it for players who are not its owner's allies."
see_also: [GapRadiusInCells, SpySat, "system:map-visibility"]
when_omitted:
  kind: value
  value: "no"
---

While a `GapGenerator=yes` structure works, the local player cannot see the ground around it unless that player is its owner or an ally of its owner. [Gap generators](/systems/map-visibility/#gap-generators) gives the conditions.

```ini title="rulesmd.ini"
[MYGAP] ; example BuildingType
GapGenerator=yes
GapRadiusInCells=10
```
