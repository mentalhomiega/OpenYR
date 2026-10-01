---
key: Weapon12
summary: "The WeaponType in position 12 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon12, Weapon12FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 12 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 12 or more. A [gattling](/systems/gattling-weapons/#stages) type fires it in place of `Weapon11` at an aircraft in the air at its sixth stage, when `Weapon2` has an anti-aircraft projectile.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=12
Weapon12=MyGun ; names the [MyGun] weapon section
```
