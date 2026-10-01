---
key: Weapon2
summary: "The WeaponType in position 2 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon2, Weapon2FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 2 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 2 or more. Position 2 stands in for the secondary weapon. A [gattling](/systems/gattling-weapons/#stages) type fires it in place of `Weapon1` at an aircraft in the air at its first stage, when this weapon has an anti-aircraft projectile.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=2
Weapon2=MyGun ; names the [MyGun] weapon section
```
