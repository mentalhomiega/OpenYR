---
key: Promote.VeteranFlash
summary: How many frames an object of this type flashes after it becomes a veteran.
see_also: [Promote.EliteFlash, VeteranFlashTimer, "system:veterancy"]
when_omitted:
  kind: inherited
  note: "[AudioVisual] VeteranFlashTimer."
---

`Promote.VeteranFlash` is an Ares key. An object of this type that becomes a veteran flashes, drawn lighter every other frame, for this many frames, in place of [`VeteranFlashTimer`](/keys/veteranflashtimer/). At 0 it does not flash. An object that skips the veteran rank and becomes elite at once flashes for [`Promote.EliteFlash`](/keys/promote.eliteflash/) instead.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Promote.VeteranFlash=30
```
