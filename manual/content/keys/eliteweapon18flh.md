---
key: EliteWeapon18FLH
summary: "The firing offset of position 18 of a numbered weapon list for an elite object."
see_also: [Weapon18FLH, EliteWeapon18, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon18FLH= value.
---

Sets the firing offset an elite object uses for position 18, in its art section, as [`Weapon18FLH`](/keys/weapon18flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 18 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon18FLH=110,0,60
```
