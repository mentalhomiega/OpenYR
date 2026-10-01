---
key: EliteStage3
summary: "The spin threshold that ends stage 3 of an elite object's gattling weapon."
see_also: [Stage3, WeaponStages, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: "0"
---

Takes the place of [`Stage3`](/keys/stage3/) for an elite object. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 3.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
Stage3=600
EliteStage3=300 ; an elite object spins up twice as fast
```
