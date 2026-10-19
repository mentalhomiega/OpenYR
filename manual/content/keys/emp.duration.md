---
key: EMP.Duration
summary: "Makes a warhead stun the objects its blast damages for this many frames, or reactivate them when negative."
see_also: [EMP.Cap, EMP.Modifier, EMP.Threshold, EMEffect, "system:emp-pulse"]
when_omitted:
  kind: value
  value: "0, so the warhead stuns nothing through this key"
---

A warhead with a nonzero `EMP.Duration` changes the stun counter of each object within its `CellSpread` that its blast damages and whose armor has a nonzero `Verses` against it. A positive value stuns the object for that many frames and a negative value takes frames off its stun. [`EMP.Cap`](/keys/emp.cap/) sets how the value joins a counter that is already running. The warhead then makes no pulse, even with [`EMEffect=yes`](/keys/emeffect/), and its weapon's `Damage` is dealt as damage. [Warheads with EMP.Duration](/systems/emp-pulse/#warheads-with-empduration) lists which objects are reached.

```ini title="rulesmd.ini"
[MyEMPWH] ; example Warhead
EMP.Duration=150  ; ten seconds
```
