---
key: Weapon9FLH
summary: "The offset from an object's center that position 9 of its numbered weapon list fires from."
see_also: [Weapon9, EliteWeapon9FLH, PrimaryFireFLH, "system:firing-geometry"]
when_omitted:
  kind: value
  value: 0,0,0
---

Sets the firing offset of [`Weapon9`](/keys/weapon9/), in the object's art section, as [`PrimaryFireFLH`](/keys/primaryfireflh/) does for the primary weapon: `X` forward, `Y` to the gun's left and `Z` up, in leptons. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 9 or more, and the weapon fires with no barrel length or thickness.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
Weapon9FLH=100,0,60 ; a bit under half a cell forward, 60 leptons up
```
