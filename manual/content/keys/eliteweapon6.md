---
key: EliteWeapon6
summary: "The WeaponType an elite object fires in position 6 of a numbered weapon list."
see_also: [Weapon6, EliteWeapon6FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon6.
---

An elite object fires this weapon in place of [`Weapon6`](/keys/weapon6/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 6 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon6=MyGun
EliteWeapon6=MyEliteGun
```
