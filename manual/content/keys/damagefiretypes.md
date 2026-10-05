---
key: DamageFireTypes
summary: The fire animations that burn on a structure once its health is low.
see_also: [ConditionYellow, ConditionRed, CanBeOccupied, "system:structure-damage-fires"]
when_omitted:
  kind: value
  value: none
---

Each fire that burns on a badly damaged structure is an animation named in this list. The structure's art sets where the fires burn, and a type with no `DamageFireOffset0` shows none. [Structure damage fires](/systems/structure-damage-fires/) covers when they start and stop, which type each one takes, and how they are drawn.

```ini title="rulesmd.ini"
[General]
DamageFireTypes=MYFIRE1,MYFIRE2 ; AnimTypes registered in [Animations]
```

With the list empty, no structure burns, whatever its art sets.
