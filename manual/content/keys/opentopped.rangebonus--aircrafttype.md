---
key: OpenTopped.RangeBonus
scope: aircrafttype
label: 'Passenger range bonus in this transport'
see_also: [OpenToppedRangeBonus, OpenTransport.RangeBonus, OpenTopped, "system:transports"]
when_omitted:
  kind: inherited
  note: The [CombatDamage] OpenToppedRangeBonus= value.
---

`OpenTopped.RangeBonus` sets how many whole cells beyond its weapons' `Range` a passenger can fire from this [`OpenTopped=yes`](/keys/opentopped/) transport. It replaces the global [`OpenToppedRangeBonus`](/keys/opentoppedrangebonus/) for this transport only. The passenger's own [`OpenTransport.RangeBonus`](/keys/opentransport.rangebonus/) is added to it. An arcing weapon still cannot fire farther than its projectile's speed carries it. The key comes from Phobos.

```ini title="rulesmd.ini"
[BFRT] ; the stock Battle Fortress
OpenTopped.RangeBonus=4 ; passengers fire 4 cells beyond their range
```
