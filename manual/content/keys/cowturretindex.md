---
key: CowTurretIndex
summary: "The turret FV shows while it fires the weapon CowTurretWeapon names."
see_also: [CowTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "0"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`CowTurretWeapon`](/keys/cowturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `CowTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
CowTurretWeapon=2
CowTurretIndex=1
```
