---
key: EliteWeapon16FLH
summary: "The firing offset of position 16 of a numbered weapon list for an elite object."
see_also: [Weapon16FLH, EliteWeapon16, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon16FLH= value.
---

Sets the firing offset an elite object uses for position 16, in its art section, as [`Weapon16FLH`](/keys/weapon16flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 16 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon16FLH=110,0,60
```
