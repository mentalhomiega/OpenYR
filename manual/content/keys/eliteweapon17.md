---
key: EliteWeapon17
summary: "The WeaponType an elite object fires in position 17 of a numbered weapon list."
see_also: [Weapon17, EliteWeapon17FLH, "system:gattling-weapons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite weapon in this position, an elite object fires Weapon17.
---

An elite object fires this weapon in place of [`Weapon17`](/keys/weapon17/). It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 17 or more.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Weapon17=MyGun
EliteWeapon17=MyEliteGun
```
