---
key: EliteWeapon10
summary: "The WeaponType an elite object fires in position 10 of a numbered weapon list."
see_also: [Weapon10, EliteWeapon10FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon10.
---

An elite object fires this weapon in place of [`Weapon10`](/keys/weapon10/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 10 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon10=MyGun
EliteWeapon10=MyEliteGun
```
