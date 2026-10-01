---
key: Stage6
summary: "The spin threshold that ends stage 6 of a gattling weapon."
see_also: [EliteStage6, WeaponStages, RateUp, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

For a type with exactly 6 stages, the spin stops rising at this value. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 6. An elite object uses [`EliteStage6`](/keys/elitestage6/) instead.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
IsGattling=yes
WeaponStages=6
Stage6=1200
```
