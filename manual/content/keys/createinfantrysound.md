---
key: CreateInfantrySound
summary: "The sound played where any new infantryman appears when it is built."
see_also: [CreateSound]
when_omitted:
  kind: value
  value: none
---

When a house finishes building a infantryman, this sound plays where it appears, unless its type sets its own [`CreateSound`](/keys/createsound/).

```ini title="rulesmd.ini"
[AudioVisual]
CreateInfantrySound=MyBuiltSound
```
