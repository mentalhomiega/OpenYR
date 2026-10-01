---
key: EliteWeapon15FLH
summary: "The firing offset of position 15 of a numbered weapon list for an elite object."
see_also: [Weapon15FLH, EliteWeapon15, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon15FLH= value.
---

Sets the firing offset an elite object uses for position 15, in its art section, as [`Weapon15FLH`](/keys/weapon15flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 15 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon15FLH=110,0,60
```
