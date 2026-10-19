---
key: EMP.Cap
summary: "How an EMP.Duration hit combines with the stun frames an object already has."
see_also: [EMP.Duration, "system:emp-pulse"]
when_omitted:
  kind: value
  value: "-1, so a hit sets the stun to its duration unless the object already has more"
---

For a positive [`EMP.Duration`](/keys/emp.duration/), a cap above `0` adds the duration to the object's stun up to the cap and leaves a longer stun alone, `0` adds the duration without limit, and a negative cap sets the stun to the duration unless the object already has more. For a negative duration, a cap above `0` lowers the stun and then lowers it further to the cap if it is still above it, `0` ends the stun at once, and a negative cap only lowers the stun. [The table](/systems/emp-pulse/#warheads-with-empduration) gives each case.

```ini title="rulesmd.ini"
[MyEMPWH] ; example Warhead
EMP.Duration=90
EMP.Cap=300  ; repeated hits add up to 300 frames
```
