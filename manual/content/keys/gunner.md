---
key: Gunner
summary: "Makes a vehicle fire the weapon its first passenger chooses."
see_also: [IFVMode, TurretCount, "system:gunner-vehicles"]
when_omitted:
  kind: value
  value: "no"
---

A gunner vehicle with [`TurretCount`](/keys/turretcount/) above `0` that is not a gattling type fires the weapon of its [numbered list](/systems/gattling-weapons/#numbered-weapon-lists) that its first passenger's [`IFVMode`](/keys/ifvmode/) names, and the first weapon while empty. [Gunner vehicles](/systems/gunner-vehicles/) covers when the weapon changes and which turret shows.

```ini title="rulesmd.ini"
[MYIFV] ; example VehicleType
Gunner=yes
TurretCount=2
WeaponCount=3
```
