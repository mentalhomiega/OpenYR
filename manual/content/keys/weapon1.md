---
key: Weapon1
summary: "The WeaponType in position 1 of a numbered weapon list."
see_also: [TurretCount, WeaponCount, EliteWeapon1, Weapon1FLH, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: none
---

Names the weapon in position 1 of the [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) a type with [`TurretCount`](/keys/turretcount/) above `0` reads in place of `Primary` and `Secondary`. It is read only when [`WeaponCount`](/keys/weaponcount/) is 1 or more. Position 1 stands in for the primary weapon. A type that is not a gattling type fires it at every target unless a [gunner vehicle's](/systems/gunner-vehicles/) passenger chooses another weapon, and a [gattling](/systems/gattling-weapons/#stages) type fires it at its first stage.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=2
Weapon1=MyGun ; names the [MyGun] weapon section
```
