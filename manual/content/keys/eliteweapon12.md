---
key: EliteWeapon12
summary: "The WeaponType an elite object fires in position 12 of a numbered weapon list."
see_also: [Weapon12, EliteWeapon12FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon12.
---

An elite object fires this weapon in place of [`Weapon12`](/keys/weapon12/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 12 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon12=MyGun
EliteWeapon12=MyEliteGun
```
