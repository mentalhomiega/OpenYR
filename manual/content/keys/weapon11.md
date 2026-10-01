---
key: Weapon11
summary: "The WeaponType in position 11 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon11, Weapon11FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 11 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 11 or more. A [gattling](/systems/gattling-weapons/#stages) type fires it at its sixth stage.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=11
Weapon11=MyGun ; names the [MyGun] weapon section
```
