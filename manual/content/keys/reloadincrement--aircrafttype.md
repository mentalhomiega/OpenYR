---
key: ReloadIncrement
scope: aircrafttype
label: Reload delay increment
see_also: [Reload, EmptyReload, PipWrap, Ammo]
when_omitted:
  kind: value
  value: "0"
---

When a vehicle starts its reload countdown, the delay is [`Reload`](/keys/reload/) plus `ReloadIncrement` times the square of its shot count divided by [`PipWrap`](/keys/pipwrap/), dropping the remainder. With the default `PipWrap=0` the divisor is 1 and the factor is 1, so the delay is `Reload` plus `ReloadIncrement`. With `PipWrap=2` and `ReloadIncrement=10`, a vehicle holding 0 or 1 shots waits `Reload` frames, one holding 2 or 3 waits 10 frames more, and one holding 4 or 5 waits 40 frames more. A negative value shortens the delay.

```ini title="rules.ini"
[MYARTILLERY] ; a UnitType registered in [VehicleTypes]
Ammo=6
Reload=40
ReloadIncrement=10
PipWrap=2
```

A vehicle with no shots left uses [`EmptyReload`](/keys/emptyreload/) when the type sets it, and otherwise the formula above like any other shot count.
