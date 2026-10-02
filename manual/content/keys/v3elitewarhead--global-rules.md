---
key: V3EliteWarhead
scope: global-rules
label: 'Elite V3 rocket warhead'
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: none
---

The warhead a V3 rocket whose launcher was elite when it left explodes with, dealing `V3RocketEliteDamage` damage. Without a warhead it explodes without damage. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[CombatDamage]
V3EliteWarhead=MYWARHEAD
```
