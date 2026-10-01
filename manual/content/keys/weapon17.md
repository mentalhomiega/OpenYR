---
key: Weapon17
summary: "The WeaponType in position 17 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon17, Weapon17FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 17 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 17 or more. No gattling stage reaches this position.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=17
Weapon17=MyGun ; names the [MyGun] weapon section
```
