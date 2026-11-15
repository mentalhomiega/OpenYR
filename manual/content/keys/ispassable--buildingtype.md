---
key: IsPassable
scope: buildingtype
summary: "With yes, vehicles and soldiers can enter the cells of this structure."
see_also: [Foundation, Passengers]
when_omitted:
  kind: value
  value: "no"
---

A structure with `IsPassable=yes` does not block the cells of its foundation. Vehicles and soldiers move over them and plan their paths through them, and no cell of the structure counts as occupied by a building. The structure is still there to be targeted and sold, and a structure cannot be placed over it.

```ini title="rulesmd.ini"
[NATRAP] ; example BuildingType
IsPassable=yes
```
