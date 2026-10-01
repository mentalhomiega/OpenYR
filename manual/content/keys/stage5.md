---
key: Stage5
summary: "The spin threshold that ends stage 5 of a gattling weapon."
see_also: [EliteStage5, WeaponStages, RateUp, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

For a [gattling](/systems/gattling-weapons/#stages) type with more than 5 stages, the weapon moves from its fifth stage to its sixth once its spin reaches this value, and back once the spin falls below it. For a type with exactly 5 stages, the spin stops rising at this value. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 5. An elite object uses [`EliteStage5`](/keys/elitestage5/) instead.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
IsGattling=yes
WeaponStages=5
Stage5=1000
```
