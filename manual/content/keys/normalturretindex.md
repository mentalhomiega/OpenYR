---
key: NormalTurretIndex
summary: "The turret FV shows while it fires the weapon NormalTurretWeapon names."
see_also: [NormalTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "0"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`NormalTurretWeapon`](/keys/normalturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `NormalTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
NormalTurretWeapon=2
NormalTurretIndex=1
```
