---
key: MachineGunTurretIndex
summary: "The turret FV shows while it fires the weapon MachineGunTurretWeapon names."
see_also: [MachineGunTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "2"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`MachineGunTurretWeapon`](/keys/machinegunturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `MachineGunTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
MachineGunTurretWeapon=2
MachineGunTurretIndex=1
```
