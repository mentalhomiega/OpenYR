---
key: EliteStage5
summary: "The spin threshold that ends stage 5 of an elite object's gattling weapon."
see_also: [Stage5, WeaponStages, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: "0"
---

Takes the place of [`Stage5`](/keys/stage5/) for an elite object. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 5.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
Stage5=1000
EliteStage5=500 ; an elite object spins up twice as fast
```
