---
key: WeaponCount
summary: "How many positions of a numbered weapon list a type reads."
see_also: [TurretCount, Weapon1, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

A type with [`TurretCount`](/keys/turretcount/) above `0` reads positions 1 to this value of its [numbered weapon list](/systems/gattling-weapons/#numbered-weapon-lists), up to 64. Positions past it stay empty. Positions 19 to 64 are written `Weapon19` to `Weapon64`, and their elite weapons and firing offsets follow the names of the first 18.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=2
```
