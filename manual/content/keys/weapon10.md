---
key: Weapon10
summary: "The WeaponType in position 10 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon10, Weapon10FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 10 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 10 or more. A [gattling](/systems/gattling-weapons/#stages) type fires it in place of `Weapon9` at an aircraft in the air at its fifth stage, when `Weapon2` has an anti-aircraft projectile.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=10
Weapon10=MyGun ; names the [MyGun] weapon section
```
