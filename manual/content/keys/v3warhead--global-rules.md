---
key: V3Warhead
scope: global-rules
label: 'V3 rocket warhead'
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: none
---

The warhead a V3 rocket whose launcher was not elite when it left explodes with, dealing `V3RocketDamage` damage. Without a warhead it explodes without damage. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[CombatDamage]
V3Warhead=MYWARHEAD
```
