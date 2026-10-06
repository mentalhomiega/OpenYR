---
key: OpenTransport.RangeBonus
scope: aircrafttype
label: 'Own range bonus when riding open-topped'
see_also: [OpenTopped.RangeBonus, OpenToppedRangeBonus, OpenTopped, "system:transports"]
when_omitted:
  kind: inherited
  note: The [CombatDamage] OpenTransport.RangeBonus= value.
---

`OpenTransport.RangeBonus` adds whole cells to this object's weapon range while it fires from any [`OpenTopped=yes`](/keys/opentopped/) transport. It is added to the transport's [`OpenTopped.RangeBonus`](/keys/opentopped.rangebonus/), so a passenger with a value of 2 in a transport that gives 2 fires 4 cells beyond its range. The key comes from Phobos.

```ini title="rulesmd.ini"
[E1] ; the stock GI
OpenTransport.RangeBonus=1
```
