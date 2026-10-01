---
key: Stage4
summary: "The spin threshold that ends stage 4 of a gattling weapon."
see_also: [EliteStage4, WeaponStages, RateUp, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

For a [gattling](/systems/gattling-weapons/#stages) type with more than 4 stages, the weapon moves from its fourth stage to its fifth once its spin reaches this value, and back once the spin falls below it. For a type with exactly 4 stages, the spin stops rising at this value. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 4. An elite object uses [`EliteStage4`](/keys/elitestage4/) instead.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
IsGattling=yes
WeaponStages=4
Stage4=800
```
