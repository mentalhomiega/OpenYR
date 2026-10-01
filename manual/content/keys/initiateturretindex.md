---
key: InitiateTurretIndex
summary: "The turret FV shows while it fires the weapon InitiateTurretWeapon names."
see_also: [InitiateTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "0"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`InitiateTurretWeapon`](/keys/initiateturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `InitiateTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
InitiateTurretWeapon=2
InitiateTurretIndex=1
```
