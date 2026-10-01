---
key: EliteWeapon12FLH
summary: "The firing offset of position 12 of a numbered weapon list for an elite object."
see_also: [Weapon12FLH, EliteWeapon12, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon12FLH= value.
---

Sets the firing offset an elite object uses for position 12, in its art section, as [`Weapon12FLH`](/keys/weapon12flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 12 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon12FLH=110,0,60
```
