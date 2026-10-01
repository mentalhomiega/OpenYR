---
key: ChronoTurretIndex
summary: "The turret FV shows while it fires the weapon ChronoTurretWeapon names."
see_also: [ChronoTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "0"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`ChronoTurretWeapon`](/keys/chronoturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `ChronoTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
ChronoTurretWeapon=2
ChronoTurretIndex=1
```
