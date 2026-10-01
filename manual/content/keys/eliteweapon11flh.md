---
key: EliteWeapon11FLH
summary: "The firing offset of position 11 of a numbered weapon list for an elite object."
see_also: [Weapon11FLH, EliteWeapon11, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon11FLH= value.
---

Sets the firing offset an elite object uses for position 11, in its art section, as [`Weapon11FLH`](/keys/weapon11flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 11 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon11FLH=110,0,60
```
