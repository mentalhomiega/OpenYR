---
key: EliteWeapon14FLH
summary: "The firing offset of position 14 of a numbered weapon list for an elite object."
see_also: [Weapon14FLH, EliteWeapon14, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon14FLH= value.
---

Sets the firing offset an elite object uses for position 14, in its art section, as [`Weapon14FLH`](/keys/weapon14flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 14 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon14FLH=110,0,60
```
