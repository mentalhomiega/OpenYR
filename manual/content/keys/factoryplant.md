---
key: FactoryPlant
summary: Makes this structure lower its owner's prices by its CostBonus factors.
see_also: [InfantryCostBonus, UnitsCostBonus, AircraftCostBonus, BuildingsCostBonus, DefensesCostBonus, "system:production"]
when_omitted:
  kind: value
  value: "no"
---

While a `FactoryPlant=yes` structure stands on the map, its owner's prices are multiplied by its [`InfantryCostBonus`](/keys/infantrycostbonus/), [`UnitsCostBonus`](/keys/unitscostbonus/), [`AircraftCostBonus`](/keys/aircraftcostbonus/), [`BuildingsCostBonus`](/keys/buildingscostbonus/) or [`DefensesCostBonus`](/keys/defensescostbonus/), by the kind of object bought. [What a house pays](/keys/cost/#what-a-house-pays) gives the whole price.

```ini title="rulesmd.ini"
[MYPLANT] ; example BuildingType
FactoryPlant=yes
UnitsCostBonus=0.75 ; vehicles cost a quarter less
```

The structure counts from the moment it is placed until it is sold or destroyed, whether or not its owner has power. A captured plant lowers the prices of its new owner.
