---
key: EliteWeapon5FLH
summary: "The firing offset of position 5 of a numbered weapon list for an elite object."
see_also: [Weapon5FLH, EliteWeapon5, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon5FLH= value.
---

Sets the firing offset an elite object uses for position 5, in its art section, as [`Weapon5FLH`](/keys/weapon5flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 5 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon5FLH=110,0,60
```
