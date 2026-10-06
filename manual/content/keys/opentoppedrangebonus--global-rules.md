---
key: OpenToppedRangeBonus
scope: global-rules
label: 'Open-topped passenger range bonus'
see_also: [OpenToppedDamageMultiplier, OpenTopped, "system:transports"]
when_omitted:
  kind: value
  value: "0"
---

A passenger firing from an [`OpenTopped=yes`](/keys/opentopped/) transport can fire at targets this many whole cells beyond its weapons' `Range`. An arcing weapon gains the bonus too, but still cannot fire farther than its projectile's speed carries it. A transport's own [`OpenTopped.RangeBonus`](/keys/opentopped.rangebonus/) replaces this value for that transport.

```ini title="rulesmd.ini"
[CombatDamage]
OpenToppedRangeBonus=2
```
