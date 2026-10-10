---
key: MobileFire
summary: Stops a vehicle firing while it has a move order, and quarters its firing delay when it halts.
see_also: ["NoMovingFire", "Primary"]
when_omitted:
  kind: value
  value: "yes"
---

`MobileFire=no` stops a vehicle firing while it has a move order. A vehicle with a move order still heading somewhere, including one held up by a blockage, cannot fire. It can fire again once the order completes or is cleared.

When the vehicle halts, its firing delay is replaced by a quarter of its secondary weapon's delay. [`NoMovingFire=yes`](/keys/nomovingfire/) describes that delay and its rounding. The replacement discards any delay still running.

`MobileFire=no` does not let the vehicle reload by itself. That is the other effect of `NoMovingFire=yes`. The key has no effect on infantry, buildings, or aircraft.

```ini title="rules.ini"
[MYTANK] ; a UnitType registered in [VehicleTypes]
MobileFire=no ; MYTANK fires only from a standstill
```
