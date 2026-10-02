---
key: CMislWarhead
scope: global-rules
label: 'Boomer missile warhead'
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: none
---

The warhead a Boomer missile whose launcher was not elite when it left explodes with, dealing `CMislDamage` damage. Without a warhead it explodes without damage. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[CombatDamage]
CMislWarhead=MYWARHEAD
```
