---
key: Weapon15
summary: "The WeaponType in position 15 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon15, Weapon15FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 15 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 15 or more. No gattling stage reaches this position.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=15
Weapon15=MyGun ; names the [MyGun] weapon section
```
