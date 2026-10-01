---
key: MinDamage
summary: The smallest damage a warhead may deal to a target close to the impact.
when_omitted:
  kind: value
  value: "1"
---

The game reads this value and never uses it. A blast's damage can fall to zero with distance or through the warhead's [`Verses`](/keys/verses/), with no floor. [What the target loses](/systems/warheads/#what-the-target-loses) lists the steps.

```ini title="rulesmd.ini"
[CombatDamage]
MinDamage=1 ; has no effect
```
