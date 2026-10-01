---
key: TurretCount
summary: "Makes a type read its weapons from a numbered list when above 0."
see_also: [WeaponCount, Weapon1, IsGattling, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "0"
---

A type with a value above `0` reads its weapons from [`Weapon1`](/keys/weapon1/) to `Weapon18`, as many as [`WeaponCount`](/keys/weaponcount/) says, and their offsets from `Weapon1FLH` and the rest. It then ignores `Primary`, `Secondary`, `ElitePrimary`, `EliteSecondary` and their art offsets. [Numbered weapon lists](/systems/gattling-weapons/#numbered-weapon-lists) covers the list. A vehicle type that is not a gattling type also loads this many [numbered turrets](/systems/gunner-vehicles/#the-turret).

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
TurretCount=1
WeaponCount=2
Weapon1=MyGun
Weapon2=MyFlakGun
```
