---
key: CMislDamage
scope: global-rules
summary: "Damage the CMisl missile deals where it explodes when its launcher is not elite, with the warhead set in [CombatDamage]."
see_also: ["system:spawned-aircraft"]
when_omitted:
  kind: value
  value: "500"
---

Damage the CMisl missile deals where it explodes when its launcher is not elite, with the warhead set in `[CombatDamage]`. This is a `[General]` key and applies to the aircraft type named by `CMislType`. [Missiles](/systems/spawned-aircraft/#missiles) covers the flight.

```ini title="rulesmd.ini"
[General]
CMislDamage=500
```
