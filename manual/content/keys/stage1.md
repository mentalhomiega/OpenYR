---
key: Stage1
summary: "The spin threshold that ends stage 1 of a gattling weapon."
see_also: [EliteStage1, WeaponStages, RateUp, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

For a [gattling](/systems/gattling-weapons/#stages) type with more than 1 stages, the weapon moves from its first stage to its second once its spin reaches this value, and back once the spin falls below it. For a type with exactly 1 stages, the spin stops rising at this value. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 2. An elite object uses [`EliteStage1`](/keys/elitestage1/) instead.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
IsGattling=yes
WeaponStages=2
Stage1=200
```
