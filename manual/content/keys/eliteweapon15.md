---
key: EliteWeapon15
summary: "The WeaponType an elite object fires in position 15 of a numbered weapon list."
see_also: [Weapon15, EliteWeapon15FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon15.
---

An elite object fires this weapon in place of [`Weapon15`](/keys/weapon15/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 15 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon15=MyGun
EliteWeapon15=MyEliteGun
```
