---
key: Weapon3FLH
summary: "The offset from an object's center that position 3 of its numbered weapon list fires from."
see_also: [Weapon3, EliteWeapon3FLH, PrimaryFireFLH, "system:firing-geometry"]
when_omitted:
  kind: value
  value: 0,0,0
---

Sets the firing offset of [`Weapon3`](/keys/weapon3/), in the object's art section, as [`PrimaryFireFLH`](/keys/primaryfireflh/) does for the primary weapon: `X` forward, `Y` to the gun's left and `Z` up, in leptons. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 3 or more, and the weapon fires with no barrel length or thickness.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
Weapon3FLH=100,0,60 ; a bit under half a cell forward, 60 leptons up
```
