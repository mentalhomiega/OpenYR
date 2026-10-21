---
key: BurstDelay0
summary: The gap in frames between the first and second shot of a vehicle's burst.
see_also: [Burst, ROF, BurstDelay1, BurstDelay2, BurstDelay3]
when_omitted:
  kind: value
  value: "-1"
---

`BurstDelay0` is the wait, in game frames, after the first shot of a vehicle's burst and before the second. `-1` gives a random three to five frames instead. It is set on the vehicle, not on the weapon, and covers the bursts of every weapon the vehicle fires.

```ini title="rules.ini"
[MyTank] ; example VehicleType
Primary=MyCannon ; a weapon with Burst=2
BurstDelay0=4 ; four frames between the first shot and the second
```

The wait applies only when the weapon's [`Burst`](/keys/burst/) is above `1`. With `Burst=1`, the first shot is the last of the burst and is followed by [`ROF`](/keys/rof/#scope-weapontype) instead.

Any value other than `-1` is used as written. The house rate-of-fire bias, the random extra frames and the veteran rate-of-fire ability change only the `ROF` wait. Infantry, aircraft and structures have no such setting, so every gap in their bursts is a random three to five frames. [`Burst`](/keys/burst/) lists the weapons and structures that never use the short gap.
