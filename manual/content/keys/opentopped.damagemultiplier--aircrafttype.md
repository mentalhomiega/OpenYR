---
key: OpenTopped.DamageMultiplier
scope: aircrafttype
label: 'Passenger damage multiplier in this transport'
see_also: [OpenToppedDamageMultiplier, OpenTransport.DamageMultiplier, OpenTopped, "system:transports"]
when_omitted:
  kind: inherited
  note: The [CombatDamage] OpenToppedDamageMultiplier= value.
---

`OpenTopped.DamageMultiplier` multiplies the damage of each shot a passenger fires from this [`OpenTopped=yes`](/keys/opentopped/) transport. It replaces the global [`OpenToppedDamageMultiplier`](/keys/opentoppeddamagemultiplier/) for this transport only. The passenger's own [`OpenTransport.DamageMultiplier`](/keys/opentransport.damagemultiplier/) multiplies the result too, and the product is rounded down to a whole number. The key comes from Phobos.

```ini title="rulesmd.ini"
[BFRT] ; the stock Battle Fortress
OpenTopped.DamageMultiplier=1.5 ; passengers do half again as much damage
```
