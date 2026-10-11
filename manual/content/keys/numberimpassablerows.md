---
key: NumberImpassableRows
summary: Limits the blocking of a structure to the first columns of its foundation for the vehicles that may enter it.
see_also: [Foundation, UnitRepair, Bunker, Bib]
when_omitted:
  kind: value
  value: "-1"
---

`NumberImpassableRows` counts the columns of the structure's [`Foundation`](/keys/foundation/#scope-buildingtype), starting at the west corner, that stop a vehicle which may enter the structure. The columns after them do not stop it, so the vehicle drives into the back of the structure. The default of `-1` keeps every column blocking.

The count applies to a vehicle only in these cases:

- The vehicle is in radio contact with the structure, as a harvester is with the refinery it is docking at.
- The structure has [`UnitRepair=yes`](/keys/unitrepair/) or [`Bunker=yes`](/keys/bunker/#scope-buildingtype). Any vehicle can then cross its later columns.

A `Bunker=yes` structure holding a vehicle blocks every column whatever the value. A `NumberImpassableRows=0` bunker is open on every column while it is empty. Infantry and aircraft ignore the key.

Stock rules set it to `3` on the refineries, naval yards, and `CAOUTP`, to `1` on the weapons factories and service depots, and to `0` on `NATBNK`.

```ini title="rules.ini"
[MYDEPOT] ; a BuildingType registered in [BuildingTypes]
Foundation=3x3
UnitRepair=yes
NumberImpassableRows=1 ; only the west column stops a vehicle
```
