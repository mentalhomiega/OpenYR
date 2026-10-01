---
key: EliteWeapon3FLH
summary: "The firing offset of position 3 of a numbered weapon list for an elite object."
see_also: [Weapon3FLH, EliteWeapon3, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon3FLH= value.
---

Sets the firing offset an elite object uses for position 3, in its art section, as [`Weapon3FLH`](/keys/weapon3flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 3 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon3FLH=110,0,60
```
