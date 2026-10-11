---
key: LeaveGrinderSound
scope: global-rules
label: Grinder leave sound
see_also: [Grinding, EnterGrinderSound]
when_omitted:
  kind: value
  value: none
---

Accepted in `[AudioVisual]`, but nothing plays it. A grinder destroys what it takes in, so no object leaves it.

```ini title="rulesmd.ini"
[AudioVisual]
LeaveGrinderSound=
```
