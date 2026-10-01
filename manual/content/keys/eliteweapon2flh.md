---
key: EliteWeapon2FLH
summary: "The firing offset of position 2 of a numbered weapon list for an elite object."
see_also: [Weapon2FLH, EliteWeapon2, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon2FLH= value.
---

Sets the firing offset an elite object uses for position 2, in its art section, as [`Weapon2FLH`](/keys/weapon2flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 2 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon2FLH=110,0,60
```
