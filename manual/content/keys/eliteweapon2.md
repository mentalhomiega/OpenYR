---
key: EliteWeapon2
summary: "The WeaponType an elite object fires in position 2 of a numbered weapon list."
see_also: [Weapon2, EliteWeapon2FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon2.
---

An elite object fires this weapon in place of [`Weapon2`](/keys/weapon2/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 2 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon2=MyGun
EliteWeapon2=MyEliteGun
```
