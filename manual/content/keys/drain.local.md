---
key: Drain.Local
summary: "With yes, a drain involving this object takes only the drained power plant's own power."
see_also: [Drain.Amount, DrainWeapon, Drainable]
when_omitted:
  kind: value
  value: "no"
---

`Drain.Local` is an Ares key, read on the draining object and on the drained power plant. When either sets `yes`, draining the plant takes only that plant's power, limited by [`Drain.Amount`](/keys/drain.amount/), from its owner's total. When both leave it at `no`, a drained plant leaves its owner with no power at all, as in Yuri's Revenge. The owner's power does not fall below zero.

A local drain does not switch off its victim's `PoweredSpecial` structures, which a full drain does. The money a drained refinery pays is unchanged.

```ini title="rulesmd.ini"
[MYDISC] ; example VehicleType
Drain.Local=yes
Drain.Amount=100
```
