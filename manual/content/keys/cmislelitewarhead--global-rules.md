---
key: CMislEliteWarhead
scope: global-rules
label: 'Elite Boomer missile warhead'
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: none
---

The warhead a Boomer missile whose launcher was elite when it left explodes with, dealing `CMislEliteDamage` damage. Without a warhead it explodes without damage. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[CombatDamage]
CMislEliteWarhead=MYWARHEAD
```
