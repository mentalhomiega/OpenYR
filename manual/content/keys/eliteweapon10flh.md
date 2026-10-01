---
key: EliteWeapon10FLH
summary: "The firing offset of position 10 of a numbered weapon list for an elite object."
see_also: [Weapon10FLH, EliteWeapon10, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon10FLH= value.
---

Sets the firing offset an elite object uses for position 10, in its art section, as [`Weapon10FLH`](/keys/weapon10flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 10 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon10FLH=110,0,60
```
