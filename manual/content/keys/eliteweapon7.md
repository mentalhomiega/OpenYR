---
key: EliteWeapon7
summary: "The WeaponType an elite object fires in position 7 of a numbered weapon list."
see_also: [Weapon7, EliteWeapon7FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon7.
---

An elite object fires this weapon in place of [`Weapon7`](/keys/weapon7/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 7 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon7=MyGun
EliteWeapon7=MyEliteGun
```
