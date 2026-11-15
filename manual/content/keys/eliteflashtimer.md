---
key: EliteFlashTimer
summary: How many frames an object flashes after it becomes elite.
see_also: [UpgradeEliteSound, "system:veterancy"]
when_omitted:
  kind: value
  value: "100"
---

An object of any house that becomes elite flashes, drawn lighter every other frame, for this many frames. An object created elite does not flash. A type's [`Promote.EliteFlash`](/keys/promote.eliteflash/) replaces this value for that type.

```ini title="rulesmd.ini"
[AudioVisual]
EliteFlashTimer=100
```
