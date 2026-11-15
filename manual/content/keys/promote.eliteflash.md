---
key: Promote.EliteFlash
summary: How many frames an object of this type flashes after it becomes elite.
see_also: [Promote.VeteranFlash, EliteFlashTimer, "system:veterancy"]
when_omitted:
  kind: inherited
  note: "[AudioVisual] EliteFlashTimer."
---

`Promote.EliteFlash` is an Ares key. An object of this type that becomes elite flashes, drawn lighter every other frame, for this many frames, in place of [`EliteFlashTimer`](/keys/eliteflashtimer/). At 0 it does not flash.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Promote.EliteFlash=60
```
