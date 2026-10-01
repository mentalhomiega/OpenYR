---
key: EliteWeapon11
summary: "The WeaponType an elite object fires in position 11 of a numbered weapon list."
see_also: [Weapon11, EliteWeapon11FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon11.
---

An elite object fires this weapon in place of [`Weapon11`](/keys/weapon11/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 11 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon11=MyGun
EliteWeapon11=MyEliteGun
```
