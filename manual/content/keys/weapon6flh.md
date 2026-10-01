---
key: Weapon6FLH
summary: "The offset from an object's center that position 6 of its numbered weapon list fires from."
see_also: [Weapon6, EliteWeapon6FLH, PrimaryFireFLH, "system:firing-geometry"]
when_omitted:
  kind: value
  value: 0,0,0
---

Sets the firing offset of [`Weapon6`](/keys/weapon6/), in the object's art section, as [`PrimaryFireFLH`](/keys/primaryfireflh/) does for the primary weapon: `X` forward, `Y` to the gun's left and `Z` up, in leptons. It is read only for a type with [`TurretCount`](/keys/turretcount/) above `0` and [`WeaponCount`](/keys/weaponcount/) of 6 or more, and the weapon fires with no barrel length or thickness.

```ini title="art.ini"
[MYTANK] ; the Image ID of a UnitType
Weapon6FLH=100,0,60 ; a bit under half a cell forward, 60 leptons up
```
