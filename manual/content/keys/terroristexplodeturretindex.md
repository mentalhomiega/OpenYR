---
key: TerroristExplodeTurretIndex
summary: "The turret FV shows while it fires the weapon TerroristExplodeTurretWeapon names."
see_also: [TerroristExplodeTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "0"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`TerroristExplodeTurretWeapon`](/keys/terroristexplodeturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `TerroristExplodeTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
TerroristExplodeTurretWeapon=2
TerroristExplodeTurretIndex=1
```
