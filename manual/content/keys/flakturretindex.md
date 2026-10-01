---
key: FlakTurretIndex
summary: "The turret FV shows while it fires the weapon FlakTurretWeapon names."
see_also: [FlakTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "3"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`FlakTurretWeapon`](/keys/flakturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `FlakTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
FlakTurretWeapon=2
FlakTurretIndex=1
```
