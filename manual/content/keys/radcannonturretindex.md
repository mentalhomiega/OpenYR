---
key: RadCannonTurretIndex
summary: "The turret FV shows while it fires the weapon RadCannonTurretWeapon names."
see_also: [RadCannonTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "0"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`RadCannonTurretWeapon`](/keys/radcannonturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `RadCannonTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
RadCannonTurretWeapon=2
RadCannonTurretIndex=1
```
