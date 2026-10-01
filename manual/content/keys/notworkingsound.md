---
key: NotWorkingSound
summary: "The sound played at a structure when it drops out of service."
see_also: [WorkingSound, "system:power"]
when_omitted:
  kind: value
  value: none
---

When a structure of this type stops being [operational](/systems/power/#defenses), or starts being sold, after it was in service, this sound plays at it.

```ini title="rulesmd.ini"
[MYDEFENSE] ; example BuildingType
NotWorkingSound=PowerOff
```
