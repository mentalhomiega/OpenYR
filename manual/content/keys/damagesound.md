---
key: DamageSound
summary: "The sound played where an object of this type takes a hit that does not change its condition."
see_also: [BuildingDamageSound]
when_omitted:
  kind: value
  value: none
---

When a hit damages an object of this type without taking it below half strength, into the red or to destruction, this sound plays where it stands. A structure with its own `DamageSound` does not play the rules' [`BuildingDamageSound`](/keys/buildingdamagesound/).

```ini title="rulesmd.ini"
[MYBARREL] ; example BuildingType
DamageSound=BuildingMetalDamaged
```
