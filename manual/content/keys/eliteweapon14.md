---
key: EliteWeapon14
summary: "The WeaponType an elite object fires in position 14 of a numbered weapon list."
see_also: [Weapon14, EliteWeapon14FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon14.
---

An elite object fires this weapon in place of [`Weapon14`](/keys/weapon14/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 14 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon14=MyGun
EliteWeapon14=MyEliteGun
```
