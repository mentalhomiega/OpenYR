---
key: BrainBlastTurretIndex
summary: "The turret FV shows while it fires the weapon BrainBlastTurretWeapon names."
see_also: [BrainBlastTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "0"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`BrainBlastTurretWeapon`](/keys/brainblastturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `BrainBlastTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
BrainBlastTurretWeapon=2
BrainBlastTurretIndex=1
```
