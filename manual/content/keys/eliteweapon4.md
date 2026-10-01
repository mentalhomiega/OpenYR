---
key: EliteWeapon4
summary: "The WeaponType an elite object fires in position 4 of a numbered weapon list."
see_also: [Weapon4, EliteWeapon4FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon4.
---

An elite object fires this weapon in place of [`Weapon4`](/keys/weapon4/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 4 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon4=MyGun
EliteWeapon4=MyEliteGun
```
