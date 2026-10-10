---
key: CostAircraftMult
summary: The factor a house of this country pays for aircraft.
see_also: [CostInfantryMult, CostUnitsMult, AircraftCostBonus, Cost, "system:production"]
when_omitted:
  kind: value
  value: "1.0"
---

A house of this country pays the price of aircraft times this value, in every game type. It combines with [factory plant](/keys/factoryplant/) bonuses, as [What a house pays](/keys/cost/#what-a-house-pays) describes, and any fraction is dropped at the end.

```ini title="rulesmd.ini"
[MYCOUNTRY] ; example country
CostAircraftMult=0.9
```

Build times do not change.
