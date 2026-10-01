---
key: IvanTimedDelay
summary: "How many frames after it is planted an Ivan bomb goes off."
see_also: [IvanDamage, IvanIconFlickerRate, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: "0"
---

An [Ivan bomb](/systems/ivan-bombs/#the-countdown) goes off this many frames after it is planted, and its icon counts down over the same time.

```ini title="rulesmd.ini"
[CombatDamage]
IvanTimedDelay=450
```
