---
key: WeaponCount
summary: "How many positions of a numbered weapon list a type reads."
see_also: [TurretCount, Weapon1, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

A type with [`TurretCount`](/keys/turretcount/) above `0` reads positions 1 to this value of its [numbered weapon list](/systems/gattling-weapons/#numbered-weapon-lists), up to 18. Positions past it stay empty.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=2
```
