---
key: MinDebris
summary: The least wreckage the object throws when destroyed, when MaxDebris allows any.
see_also: [MaxDebris, DebrisTypes, DebrisAnims]
when_omitted:
  kind: value
  value: "0"
---

A destroyed object whose [`MaxDebris`](/keys/maxdebris/) is above `0` throws a random number of pieces from `MinDebris` up to one less than `MaxDebris`. `MaxDebris` describes which pieces are thrown.

```ini title="rulesmd.ini"
[MyTank] ; example VehicleType
MinDebris=2
MaxDebris=6
```
