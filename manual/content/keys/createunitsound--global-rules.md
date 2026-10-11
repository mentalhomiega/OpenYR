---
key: CreateUnitSound
scope: global-rules
label: Vehicle create sound
see_also: [CreateSound]
when_omitted:
  kind: value
  value: none
---

When a house finishes building a vehicle, this sound plays where it appears, unless its type sets its own [`CreateSound`](/keys/createsound/).

```ini title="rulesmd.ini"
[AudioVisual]
CreateUnitSound=MyBuiltSound
```
