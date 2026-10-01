---
key: EliteOccupyWeapon
summary: The WeaponType this soldier fires from inside a garrisoned structure once it is elite.
see_also: [OccupyWeapon, Occupier, ElitePrimary, "system:garrisons", "system:veterancy"]
when_omitted:
  kind: value
  value: none
  note: With no elite garrison weapon, an elite soldier fires its elite primary weapon from inside the structure, or its primary weapon when it has no elite primary.
---

An elite soldier uses this weapon in place of its [`OccupyWeapon`](/keys/occupyweapon/) when its turn to fire comes in a garrison. The elite weapon does not fall back to `OccupyWeapon`: an elite soldier whose type sets only `OccupyWeapon` fires its primary weapon from inside.

```ini title="rulesmd.ini"
[MYRIFLEMAN] ; example InfantryType
Occupier=yes
OccupyWeapon=MyGarrisonGun
EliteOccupyWeapon=MyEliteGarrisonGun
```
