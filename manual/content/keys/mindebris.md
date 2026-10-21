---
key: MinDebris
summary: The least wreckage a destroyed object, or a warhead's shot, throws when MaxDebris allows any.
see_also: [MaxDebris, DebrisTypes, DebrisAnims]
---

A destroyed object whose [`MaxDebris`](/keys/maxdebris/) is above `0` throws a random number of pieces from `MinDebris` up to one less than `MaxDebris`. `MaxDebris` describes which pieces are thrown. A value below `0` is read as `0`. A `MaxDebris` below `MinDebris` is raised to `MinDebris`, so when the two are equal, the count is drawn from one less than `MinDebris` up to `MinDebris`.

```ini title="rulesmd.ini"
[MyTank] ; example VehicleType
MinDebris=2
MaxDebris=6
```

A warhead's `MinDebris` sets the least debris its shot throws. The [warhead page](/keys/mindebris/#scope-warheadtype) gives its default and details.
