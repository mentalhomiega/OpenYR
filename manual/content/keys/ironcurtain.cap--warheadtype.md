---
key: IronCurtain.Cap
scope: warheadtype
summary: "How an IronCurtain.Duration hit combines with the curtain time an object already has."
see_also: ["IronCurtain.Duration", "EMP.Cap"]
when_omitted:
  kind: value
  value: "-1, so a hit sets the curtain to its duration unless the object already has more"
---

For a positive [`IronCurtain.Duration`](/keys/ironcurtain.duration--warheadtype/), a cap above `0` adds the duration to the time the object has left, up to the cap, and leaves a longer time alone. `0` adds the duration without limit, and a negative cap sets the time to the duration unless the object already has more.

For a negative duration, `0` ends the curtain at once, a cap above `0` lowers the time and then to the cap if it is still above it, and a negative cap only lowers the time. A Force Shield counts as a curtain only for a negative duration; a positive duration replaces it.

```ini title="rulesmd.ini"
[MyGuardWH] ; example Warhead
IronCurtain.Duration=150
IronCurtain.Cap=900  ; repeated hits add up to 900 frames
```
