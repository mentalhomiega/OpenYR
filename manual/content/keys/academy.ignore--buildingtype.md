---
key: Academy.Ignore
scope: buildingtype
summary: "The types a structure's Academy levels never reach."
see_also: ["Academy.Types", "Academy.InfantryVeterancy"]
when_omitted:
  kind: value
  value: "none"
---

A list of type names that the structure's `Academy.` levels skip, whatever [`Academy.Types`](/keys/academy.types/) lists.

```ini title="rulesmd.ini"
[GAACAD] ; example BuildingType
Academy.InfantryVeterancy=1.0
Academy.Ignore=E2
```
