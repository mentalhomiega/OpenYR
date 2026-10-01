---
key: EliteWeapon4FLH
summary: "The firing offset of position 4 of a numbered weapon list for an elite object."
see_also: [Weapon4FLH, EliteWeapon4, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon4FLH= value.
---

Sets the firing offset an elite object uses for position 4, in its art section, as [`Weapon4FLH`](/keys/weapon4flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 4 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon4FLH=110,0,60
```
