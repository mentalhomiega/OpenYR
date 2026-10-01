---
key: Weapon3
summary: "The WeaponType in position 3 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon3, Weapon3FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 3 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 3 or more. A [gattling](/systems/gattling-weapons/#stages) type fires it at its second stage.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=3
Weapon3=MyGun ; names the [MyGun] weapon section
```
