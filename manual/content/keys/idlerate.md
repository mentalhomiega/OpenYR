---
key: IdleRate
summary: "Makes a stopped object keep stepping its animation count, once every this many game frames."
see_also: [WalkRate]
when_omitted:
  kind: value
  value: "0"
---

```ini title="rulesmd.ini"
[DLPH] ; example VehicleType
IdleRate=8
```

With a value above `0`, a vehicle drawn from shape art whose locomotor reports it is not moving still steps its [walk animation](/keys/walkrate/), once on each game frame whose number is a multiple of `IdleRate`. It shows its walk frames while it stands, as the stock Dolphin and Giant Squid do. The step does not happen while a locomotor warhead holds the object. A moving object keeps stepping by `WalkRate`.

At `0` a stopped vehicle shows its standing, firing or dying frame.

An object that is firing shows its firing frames instead of its walk frames.
