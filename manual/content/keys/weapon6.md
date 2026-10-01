---
key: Weapon6
summary: "The WeaponType in position 6 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon6, Weapon6FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 6 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 6 or more. A [gattling](/systems/gattling-weapons/#stages) type fires it in place of `Weapon5` at an aircraft in the air at its third stage, when `Weapon2` has an anti-aircraft projectile.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=6
Weapon6=MyGun ; names the [MyGun] weapon section
```
