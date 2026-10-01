---
key: EliteWeapon5
summary: "The WeaponType an elite object fires in position 5 of a numbered weapon list."
see_also: [Weapon5, EliteWeapon5FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon5.
---

An elite object fires this weapon in place of [`Weapon5`](/keys/weapon5/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 5 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon5=MyGun
EliteWeapon5=MyEliteGun
```
