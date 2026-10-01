---
key: EliteWeapon16
summary: "The WeaponType an elite object fires in position 16 of a numbered weapon list."
see_also: [Weapon16, EliteWeapon16FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon16.
---

An elite object fires this weapon in place of [`Weapon16`](/keys/weapon16/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 16 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon16=MyGun
EliteWeapon16=MyEliteGun
```
