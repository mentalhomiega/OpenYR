---
key: EliteStage4
summary: "The spin threshold that ends stage 4 of an elite object's gattling weapon."
see_also: [Stage4, WeaponStages, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: "0"
---

Takes the place of [`Stage4`](/keys/stage4/) for an elite object. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 4.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
Stage4=800
EliteStage4=400 ; an elite object spins up twice as fast
```
