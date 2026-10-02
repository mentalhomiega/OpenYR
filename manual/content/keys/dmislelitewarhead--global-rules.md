---
key: DMislEliteWarhead
scope: global-rules
label: 'Elite Dreadnought missile warhead'
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: none
---

The warhead a Dreadnought missile whose launcher was elite when it left explodes with, dealing `DMislEliteDamage` damage. Without a warhead it explodes without damage. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[CombatDamage]
DMislEliteWarhead=MYWARHEAD
```
