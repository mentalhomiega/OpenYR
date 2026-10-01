---
key: RepairTurretIndex
summary: "The turret FV shows while it fires the weapon RepairTurretWeapon names."
see_also: [RepairTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "1"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`RepairTurretWeapon`](/keys/repairturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `RepairTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
RepairTurretWeapon=2
RepairTurretIndex=1
```
