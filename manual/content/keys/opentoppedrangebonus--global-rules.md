---
key: OpenToppedRangeBonus
scope: global-rules
label: 'Open-topped passenger range bonus'
see_also: [OpenToppedDamageMultiplier, OpenTopped, "system:transports"]
when_omitted:
  kind: value
  value: "0"
---

A passenger firing from an [`OpenTopped=yes`](/keys/opentopped/) transport can fire at targets this many whole cells beyond its weapons' `Range`. Weapons with arcing projectiles do not gain the bonus, because their reach depends on the projectile's speed.

```ini title="rulesmd.ini"
[CombatDamage]
OpenToppedRangeBonus=2
```
