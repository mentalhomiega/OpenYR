---
key: Weapon5
summary: "The WeaponType in position 5 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon5, Weapon5FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 5 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 5 or more. A [gattling](/systems/gattling-weapons/#stages) type fires it at its third stage.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=5
Weapon5=MyGun ; names the [MyGun] weapon section
```
