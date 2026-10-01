---
key: WeaponStages
summary: "How many stages a gattling weapon has."
see_also: [IsGattling, Stage1, EliteStage1, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

A [gattling](/systems/gattling-weapons/#stages) type has this many stages, up to 6, and reads that many thresholds from [`Stage1`](/keys/stage1/) on. Its spin stops rising at the last threshold. With fewer than 2 stages, no thresholds are read and the weapon stays at its first stage.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
IsGattling=yes
WeaponStages=3
```
