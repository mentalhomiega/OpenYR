---
key: EliteWeapon13
summary: "The WeaponType an elite object fires in position 13 of a numbered weapon list."
see_also: [Weapon13, EliteWeapon13FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon13.
---

An elite object fires this weapon in place of [`Weapon13`](/keys/weapon13/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 13 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon13=MyGun
EliteWeapon13=MyEliteGun
```
