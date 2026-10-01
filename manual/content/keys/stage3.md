---
key: Stage3
summary: "The spin threshold that ends stage 3 of a gattling weapon."
see_also: [EliteStage3, WeaponStages, RateUp, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

For a [gattling](/systems/gattling-weapons/#stages) type with more than 3 stages, the weapon moves from its third stage to its fourth once its spin reaches this value, and back once the spin falls below it. For a type with exactly 3 stages, the spin stops rising at this value. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 3. An elite object uses [`EliteStage3`](/keys/elitestage3/) instead.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
IsGattling=yes
WeaponStages=3
Stage3=600
```
