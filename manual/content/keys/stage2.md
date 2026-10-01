---
key: Stage2
summary: "The spin threshold that ends stage 2 of a gattling weapon."
see_also: [EliteStage2, WeaponStages, RateUp, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

For a [gattling](/systems/gattling-weapons/#stages) type with more than 2 stages, the weapon moves from its second stage to its third once its spin reaches this value, and back once the spin falls below it. For a type with exactly 2 stages, the spin stops rising at this value. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 2. An elite object uses [`EliteStage2`](/keys/elitestage2/) instead.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
IsGattling=yes
WeaponStages=2
Stage2=400
```
