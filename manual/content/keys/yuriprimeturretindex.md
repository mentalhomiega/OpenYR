---
key: YuriPrimeTurretIndex
summary: "The turret FV shows while it fires the weapon YuriPrimeTurretWeapon names."
see_also: [YuriPrimeTurretWeapon, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "0"
---

Numbers one of the `FV` vehicle's turrets: `0` is its `TUR` voxel, `1` is `TUR1`, and so on. `FV` shows that turret while it fires the weapon [`YuriPrimeTurretWeapon`](/keys/yuriprimeturretweapon/) names. Only the type named `FV` reads this key, and it has no effect while `YuriPrimeTurretWeapon` is unset.

```ini title="rulesmd.ini"
[FV]
YuriPrimeTurretWeapon=2
YuriPrimeTurretIndex=1
```
