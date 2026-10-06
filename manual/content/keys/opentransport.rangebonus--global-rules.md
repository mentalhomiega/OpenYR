---
key: OpenTransport.RangeBonus
scope: global-rules
label: "Passengers' own open-topped range bonus"
see_also: [OpenTransport.RangeBonus, OpenToppedRangeBonus, "system:transports"]
when_omitted:
  kind: value
  value: "0"
---

`OpenTransport.RangeBonus` in `[CombatDamage]` is the range bonus, in whole cells, that a passenger adds for itself when firing from an [`OpenTopped=yes`](/keys/opentopped/) transport, unless its own type sets [`OpenTransport.RangeBonus`](/keys/opentransport.rangebonus/#scope-aircrafttype). It is added to the transport's bonus. The key comes from Phobos.

```ini title="rulesmd.ini"
[CombatDamage]
OpenTransport.RangeBonus=0
```
