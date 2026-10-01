---
key: EliteWeapon13FLH
summary: "The firing offset of position 13 of a numbered weapon list for an elite object."
see_also: [Weapon13FLH, EliteWeapon13, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon13FLH= value.
---

Sets the firing offset an elite object uses for position 13, in its art section, as [`Weapon13FLH`](/keys/weapon13flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 13 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon13FLH=110,0,60
```
