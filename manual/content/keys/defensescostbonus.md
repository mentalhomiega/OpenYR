---
key: DefensesCostBonus
summary: The factor every price for defenses is multiplied by while its owner has this factory plant.
see_also: [FactoryPlant, InfantryCostBonus, CostDefensesMult, "system:production"]
when_omitted:
  kind: value
  value: "1.0"
---

While a [`FactoryPlant=yes`](/keys/factoryplant/) structure stands, its owner pays the price of a defense, a structure whose [`BuildCat`](/keys/buildcat/) is `Combat`, times this value. Each such structure applies its own factor, so two plants with `0.75` make the price `0.5625` of what it was.

```ini title="rulesmd.ini"
[MYPLANT] ; example BuildingType
FactoryPlant=yes
DefensesCostBonus=0.75
```

The key has no effect on a structure that does not set `FactoryPlant=yes`.
