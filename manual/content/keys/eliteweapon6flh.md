---
key: EliteWeapon6FLH
summary: "The firing offset of position 6 of a numbered weapon list for an elite object."
see_also: [Weapon6FLH, EliteWeapon6, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon6FLH= value.
---

Sets the firing offset an elite object uses for position 6, in its art section, as [`Weapon6FLH`](/keys/weapon6flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 6 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon6FLH=110,0,60
```
