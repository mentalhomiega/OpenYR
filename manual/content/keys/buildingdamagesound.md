---
key: BuildingDamageSound
summary: "The sound played at a structure that a hit takes below half strength or into the red."
see_also: [DamageSound]
when_omitted:
  kind: value
  value: none
---

When a hit takes a structure below half strength or into the red, this sound plays at it, unless the structure's type sets its own [`DamageSound`](/keys/damagesound/).

```ini title="rulesmd.ini"
[AudioVisual]
BuildingDamageSound=BuildingDamaged
```
