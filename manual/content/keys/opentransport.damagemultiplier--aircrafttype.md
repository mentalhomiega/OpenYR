---
key: OpenTransport.DamageMultiplier
scope: aircrafttype
label: 'Own damage multiplier when riding open-topped'
see_also: [OpenTopped.DamageMultiplier, OpenToppedDamageMultiplier, OpenTopped, "system:transports"]
when_omitted:
  kind: inherited
  note: The [CombatDamage] OpenTransport.DamageMultiplier= value.
---

`OpenTransport.DamageMultiplier` multiplies the damage of each shot this object fires from any [`OpenTopped=yes`](/keys/opentopped/) transport. The transport's [`OpenTopped.DamageMultiplier`](/keys/opentopped.damagemultiplier/) applies as well, and the product of the two is rounded down to a whole number. The key comes from Phobos.

```ini title="rulesmd.ini"
[E1] ; the stock GI
OpenTransport.DamageMultiplier=0.75
```
