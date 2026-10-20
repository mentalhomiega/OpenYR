---
key: Academy.Types
scope: buildingtype
summary: "The types a structure's Academy levels are limited to."
see_also: ["Academy.Ignore", "Academy.InfantryVeterancy"]
when_omitted:
  kind: value
  value: "none, so the levels reach every type"
---

A list of soldier, vehicle, aircraft or structure type names. When the list has an entry, the structure's `Academy.` levels reach only the listed types. The names are matched when an object is made, so a name that is no type matches nothing.

```ini title="rulesmd.ini"
[GAACAD] ; example BuildingType
Academy.InfantryVeterancy=1.0
Academy.Types=E1,GGI
```
