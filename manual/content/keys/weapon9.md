---
key: Weapon9
summary: "The WeaponType in position 9 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon9, Weapon9FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 9 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 9 or more. A [gattling](/systems/gattling-weapons/#stages) type fires it at its fifth stage.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=9
Weapon9=MyGun ; names the [MyGun] weapon section
```
