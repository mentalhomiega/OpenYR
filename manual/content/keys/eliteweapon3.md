---
key: EliteWeapon3
summary: "The WeaponType an elite object fires in position 3 of a numbered weapon list."
see_also: [Weapon3, EliteWeapon3FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon3.
---

An elite object fires this weapon in place of [`Weapon3`](/keys/weapon3/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 3 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon3=MyGun
EliteWeapon3=MyEliteGun
```
