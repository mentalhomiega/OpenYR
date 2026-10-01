---
key: IvanWarhead
summary: "The warhead an Ivan bomb explodes with."
see_also: [IvanDamage, "system:ivan-bombs"]
when_omitted:
  kind: value
  value: none
---

An [Ivan bomb](/systems/ivan-bombs/#going-off) does its damage through this warhead and plays its explosion. With none set, bombs go off harmlessly.

```ini title="rulesmd.ini"
[CombatDamage]
IvanWarhead=MyIvanWH ; a Warhead section
```
