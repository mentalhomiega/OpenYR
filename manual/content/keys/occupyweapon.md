---
key: OccupyWeapon
summary: The WeaponType this soldier fires from inside a garrisoned structure.
see_also: [EliteOccupyWeapon, Occupier, CanOccupyFire, Primary, "system:garrisons"]
when_omitted:
  kind: value
  value: none
  note: With no garrison weapon, the soldier fires its own primary weapon from inside the structure.
---

When this soldier's turn to fire comes in a [`CanOccupyFire=yes`](/keys/canoccupyfire/) structure, the structure fires this weapon. The weapon's own [`Range`](/keys/range/#scope-weapontype), damage and delay apply, modified as [Firing](/systems/garrisons/#firing) describes.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Occupier=yes
OccupyWeapon=MyGarrisonGun
EliteOccupyWeapon=MyEliteGarrisonGun
```

An elite soldier uses [`EliteOccupyWeapon`](/keys/eliteoccupyweapon/) instead. A name that matches no WeaponType creates a new, empty WeaponType of that name.
