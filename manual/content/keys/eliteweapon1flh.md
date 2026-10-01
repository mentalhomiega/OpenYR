---
key: EliteWeapon1FLH
summary: "The firing offset of position 1 of a numbered weapon list for an elite object."
see_also: [Weapon1FLH, EliteWeapon1, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon1FLH= value.
---

Sets the firing offset an elite object uses for position 1, in its art section, as [`Weapon1FLH`](/keys/weapon1flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 1 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon1FLH=110,0,60
```
