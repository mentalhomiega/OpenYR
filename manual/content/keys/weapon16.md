---
key: Weapon16
summary: "The WeaponType in position 16 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon16, Weapon16FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 16 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 16 or more. No gattling stage reaches this position.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=16
Weapon16=MyGun ; names the [MyGun] weapon section
```
