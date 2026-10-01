---
key: CanOccupyFire
summary: Lets the soldiers inside a garrisoned structure fire out of it.
see_also: [CanBeOccupied, OccupyWeapon, OccupyDamageMultiplier, OccupyROFMultiplier, OccupyWeaponRange, "system:garrisons"]
when_omitted:
  kind: value
  value: "no"
---

A [`CanBeOccupied=yes`](/keys/canbeoccupied/) structure with `CanOccupyFire=yes` attacks enemies while at least one soldier is inside. Occupants take turns firing their [`OccupyWeapon`](/keys/occupyweapon/). [Firing](/systems/garrisons/#firing) explains how the occupants' number and the `[CombatDamage]` settings change the shots.

```ini title="rulesmd.ini"
[MYOFFICES] ; example BuildingType
CanBeOccupied=yes
MaxNumberOccupants=6
CanOccupyFire=yes
```

With `CanOccupyFire=no`, soldiers can still move in and the structure still changes owner, but it does not fire. The key has no effect on a structure that does not set `CanBeOccupied=yes`.
