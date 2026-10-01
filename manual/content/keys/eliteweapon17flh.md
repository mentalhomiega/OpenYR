---
key: EliteWeapon17FLH
summary: "The firing offset of position 17 of a numbered weapon list for an elite object."
see_also: [Weapon17FLH, EliteWeapon17, "system:gattling-weapons"]
when_omitted:
  kind: inherited
  note: The same section's Weapon17FLH= value.
---

Sets the firing offset an elite object uses for position 17, in its art section, as [`Weapon17FLH`](/keys/weapon17flh/) does for the normal weapon. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 17 or more.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
EliteWeapon17FLH=110,0,60
```
