---
key: VeteranFlashTimer
summary: How many frames an object flashes after it becomes a veteran.
see_also: [EliteFlashTimer, Promote.VeteranFlash, "system:veterancy"]
when_omitted:
  kind: value
  value: "0"
---

`VeteranFlashTimer` is an Ares key in `[AudioVisual]`. An object of any house that becomes a veteran flashes, drawn lighter every other frame, for this many frames, as [`EliteFlashTimer`](/keys/eliteflashtimer/) does for elite objects. At 0 a new veteran does not flash. A type's [`Promote.VeteranFlash`](/keys/promote.veteranflash/) replaces this value for that type.

```ini title="rulesmd.ini"
[AudioVisual]
VeteranFlashTimer=45
```
