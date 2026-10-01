---
key: EliteStage6
summary: "The spin threshold that ends stage 6 of an elite object's gattling weapon."
see_also: [Stage6, WeaponStages, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: "0"
---

Takes the place of [`Stage6`](/keys/stage6/) for an elite object. It is read only for an [`IsGattling=yes`](/keys/isgattling/) type with [`WeaponStages`](/keys/weaponstages/) of at least 6.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
Stage6=1200
EliteStage6=600 ; an elite object spins up twice as fast
```
