---
key: CostUnitsMult
summary: The factor a house of this country pays for vehicles.
see_also: [CostInfantryMult, CostAircraftMult, UnitsCostBonus, Cost, "system:production"]
when_omitted:
  kind: value
  value: "1.0"
---

A house of this country pays the price of vehicles times this value, in every game type. It combines with the country's and the difficulty's [`Cost`](/keys/cost/#what-a-house-pays) multipliers and with [factory plant](/keys/factoryplant/) bonuses, and any fraction is dropped at the end.

```ini title="rulesmd.ini"
[MYCOUNTRY] ; example country
CostUnitsMult=0.9
```

Build times do not change.
