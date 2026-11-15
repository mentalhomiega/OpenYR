---
key: Academy.InfantryVeterancy
scope: buildingtype
summary: "The veterancy levels a structure gives new soldiers and Organic vehicles of its house."
see_also: ["Academy.VehicleVeterancy", "Academy.AircraftVeterancy", "Academy.BuildingVeterancy", "Academy.Types", "Academy.Ignore"]
when_omitted:
  kind: value
  value: "0, so the structure gives no veterancy"
---

A number of veterancy levels, where 1 is veteran and 2 is elite and a fraction is part of a promotion. While the house owns this structure, each new `Trainable=yes` soldier, and each new vehicle with `Organic=yes`, starts with at least that much experience. The levels cap at 2. An object that already has more, for example from [`VeteranInfantry`](/keys/veteraninfantry/#scope-housetype) or from a spy, keeps it.

When the house owns several such structures, the highest level that applies to an object counts, and the levels do not add. [`Academy.Types`](/keys/academy.types/) and [`Academy.Ignore`](/keys/academy.ignore/) limit which types a structure helps. Objects that exist before the structure do not gain anything.

```ini title="rulesmd.ini"
[GAACAD] ; example BuildingType
Academy.InfantryVeterancy=1.0
Academy.VehicleVeterancy=0.5
```
