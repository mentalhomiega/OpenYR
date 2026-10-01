---
key: CreateAircraftSound
summary: "The sound played where any new aircraft appears when it is built."
see_also: [CreateSound]
when_omitted:
  kind: value
  value: none
---

When a house finishes building a aircraft, this sound plays where it appears, unless its type sets its own [`CreateSound`](/keys/createsound/).

```ini title="rulesmd.ini"
[AudioVisual]
CreateAircraftSound=MyBuiltSound
```
