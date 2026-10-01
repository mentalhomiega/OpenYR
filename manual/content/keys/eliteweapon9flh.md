---
key: EliteWeapon9FLH
summary: "The firing offset of position 9 of a numbered weapon list for an elite object."
see_also: [Weapon9FLH, EliteWeapon9, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon9FLH= value.
---

Sets the firing offset an elite object uses for position 9, in its art section, as [`Weapon9FLH`](/keys/weapon9flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 9 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon9FLH=110,0,60
```
