---
key: EliteStage1
summary: "The spin threshold that ends stage 1 of an elite object's gattling weapon."
see_also: [Stage1, WeaponStages, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: "0"
---

Takes the place of [`Stage1`](/keys/stage1/) for an elite object. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 2.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
Stage1=200
EliteStage1=100 ; an elite object spins up twice as fast
```
