---
key: EliteWeapon7FLH
summary: "The firing offset of position 7 of a numbered weapon list for an elite object."
see_also: [Weapon7FLH, EliteWeapon7, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon7FLH= value.
---

Sets the firing offset an elite object uses for position 7, in its art section, as [`Weapon7FLH`](/keys/weapon7flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 7 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon7FLH=110,0,60
```
