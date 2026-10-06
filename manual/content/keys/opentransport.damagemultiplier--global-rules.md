---
key: OpenTransport.DamageMultiplier
scope: global-rules
label: "Passengers' own open-topped damage multiplier"
see_also: [OpenTransport.DamageMultiplier, OpenToppedDamageMultiplier, "system:transports"]
when_omitted:
  kind: value
  value: "1.0"
---

`OpenTransport.DamageMultiplier` in `[CombatDamage]` multiplies the damage of a passenger's shots from an [`OpenTopped=yes`](/keys/opentopped/) transport, unless the passenger's own type sets [`OpenTransport.DamageMultiplier`](/keys/opentransport.damagemultiplier/#scope-aircrafttype). The transport's multiplier applies as well. The key comes from Phobos.

```ini title="rulesmd.ini"
[CombatDamage]
OpenTransport.DamageMultiplier=1.0
```
