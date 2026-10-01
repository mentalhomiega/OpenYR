---
key: EliteWeapon8FLH
summary: "The firing offset of position 8 of a numbered weapon list for an elite object."
see_also: [Weapon8FLH, EliteWeapon8, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon8FLH= value.
---

Sets the firing offset an elite object uses for position 8, in its art section, as [`Weapon8FLH`](/keys/weapon8flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 8 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon8FLH=110,0,60
```
