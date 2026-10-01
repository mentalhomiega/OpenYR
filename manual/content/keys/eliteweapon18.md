---
key: EliteWeapon18
summary: "The WeaponType an elite object fires in position 18 of a numbered weapon list."
see_also: [Weapon18, EliteWeapon18FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon18.
---

An elite object fires this weapon in place of [`Weapon18`](/keys/weapon18/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 18 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon18=MyGun
EliteWeapon18=MyEliteGun
```
