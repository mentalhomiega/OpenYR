---
key: BunkerDamageMultiplier
scope: global-rules
label: 'Bunkered vehicle damage factor'
see_also: [BunkerROFMultiplier, BunkerWeaponRangeBonus, "system:tank-bunkers"]
when_omitted:
  kind: value
  value: "1.0"
---

A vehicle in a bunker multiplies the damage of each shot it fires by this value, after the owner's and the vehicle's firepower and veterancy bonuses. The product is rounded down to a whole number.

```ini title="rulesmd.ini"
[CombatDamage]
BunkerDamageMultiplier=1.3
```
