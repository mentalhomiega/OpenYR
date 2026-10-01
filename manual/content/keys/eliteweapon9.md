---
key: EliteWeapon9
summary: "The WeaponType an elite object fires in position 9 of a numbered weapon list."
see_also: [Weapon9, EliteWeapon9FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon9.
---

An elite object fires this weapon in place of [`Weapon9`](/keys/weapon9/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 9 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon9=MyGun
EliteWeapon9=MyEliteGun
```
