---
key: EliteStage2
summary: "The spin threshold that ends stage 2 of an elite object's gattling weapon."
see_also: [Stage2, WeaponStages, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: "0"
---

Takes the place of [`Stage2`](/keys/stage2/) for an elite object. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 2.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
Stage2=400
EliteStage2=200 ; an elite object spins up twice as fast
```
