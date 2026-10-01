---
key: OpenToppedDamageMultiplier
summary: "The factor applied to the damage of every shot a passenger fires from an open-topped transport."
see_also: [OpenTopped, OccupyDamageMultiplier, "system:transports"]
when_omitted:
  kind: value
  value: "1.0"
---

A passenger firing from an [open-topped transport](/systems/transports/#firing-from-an-open-topped-transport) multiplies each shot's damage by this value, after its owner's and its own firepower bonuses. The product is rounded down to a whole number.

```ini title="rulesmd.ini"
[CombatDamage]
OpenToppedDamageMultiplier=1.3
```
