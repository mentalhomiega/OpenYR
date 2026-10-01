---
key: IsGattling
summary: "Makes a type fire its numbered weapons in stages that rise as it keeps firing."
see_also: [WeaponStages, Stage1, RateUp, RateDown, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "no"
---

A gattling type fires positions 1 and 2 of its [numbered weapon list](/systems/gattling-weapons/#numbered-weapon-lists) at its first stage, 3 and 4 at its second, and so on. Its stage follows a spin that rises as it fires and falls as it idles. [Stages](/systems/gattling-weapons/#stages) covers the thresholds, and the type needs [`TurretCount`](/keys/turretcount/) above `0` to read the list.

```ini title="rulesmd.ini"
[MYGATTLING] ; example VehicleType
TurretCount=1
WeaponCount=6
IsGattling=yes
WeaponStages=3
```
