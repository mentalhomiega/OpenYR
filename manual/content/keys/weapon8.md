---
key: Weapon8
summary: "The WeaponType in position 8 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon8, Weapon8FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 8 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 8 or more. A [gattling](/systems/gattling-weapons/#stages) type fires it in place of `Weapon7` at an aircraft in the air at its fourth stage, when `Weapon2` has an anti-aircraft projectile.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=8
Weapon8=MyGun ; names the [MyGun] weapon section
```
