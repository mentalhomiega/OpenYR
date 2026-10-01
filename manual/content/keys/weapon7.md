---
key: Weapon7
summary: "The WeaponType in position 7 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon7, Weapon7FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 7 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 7 or more. A [gattling](/systems/gattling-weapons/#stages) type fires it at its fourth stage.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=7
Weapon7=MyGun ; names the [MyGun] weapon section
```
