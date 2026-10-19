---
key: EmptyReload
scope: aircrafttype
label: Empty reload delay
see_also: [Reload, ReloadIncrement, Ammo]
when_omitted:
  kind: value
  value: "-1"
  note: A value of -1 keeps the delay that [`Reload`](/keys/reload/) gives.
---

A vehicle whose ammunition has dropped to zero starts its reload countdown at `EmptyReload` frames instead of the [`Reload`](/keys/reload/) delay, and [`ReloadIncrement`](/keys/reloadincrement/) is not added. Any value other than `-1` is used, including `0`, which returns the first shot on the next frame. The shots after the first use the normal delay.

```ini title="rules.ini"
[MYARTILLERY] ; a UnitType registered in [VehicleTypes]
Ammo=4
Reload=60
EmptyReload=300 ; an emptied artillery piece takes 20 seconds to get its first shot back
```

The key has an effect only on vehicles with an `Ammo` limit.
