---
key: SniperTurretIndex
summary: "The turret FV shows while it fires the weapon SniperTurretWeapon names."
see_also: [SniperTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "0"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`SniperTurretWeapon`](/keys/sniperturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `SniperTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
SniperTurretWeapon=2
SniperTurretIndex=1
```
