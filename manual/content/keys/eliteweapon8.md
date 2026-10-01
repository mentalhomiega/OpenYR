---
key: EliteWeapon8
summary: "The WeaponType an elite object fires in position 8 of a numbered weapon list."
see_also: [Weapon8, EliteWeapon8FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon8.
---

An elite object fires this weapon in place of [`Weapon8`](/keys/weapon8/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 8 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon8=MyGun
EliteWeapon8=MyEliteGun
```
