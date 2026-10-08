---
key: IsChargeTurret
summary: "Makes a turreted vehicle show the turret that matches how much of its rearm is left."
see_also: [TurretCount, "system:gattling-weapons"]
when_omitted:
  kind: value
  value: "no"
---

A vehicle with `IsChargeTurret=yes` and a [`TurretCount`](/keys/turretcount/) above `0` shows turret `TurretCount - 1` right after it fires, and steps down one turret at a time as its weapon rearms, until it shows turret `0` once the weapon is ready. The turret number is worked out from the weapon's rearm time, not from a count of shots. A [gattling](/systems/gattling-weapons/) vehicle is not affected.

```ini title="rulesmd.ini"
[MYPRISMTANK] ; example VehicleType
TurretCount=4
IsChargeTurret=yes
```
