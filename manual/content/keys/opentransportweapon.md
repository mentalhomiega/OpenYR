---
key: OpenTransportWeapon
summary: "The weapon this object fires while riding an open-topped transport."
see_also: [OpenTopped, FireInTransport, "system:transports"]
when_omitted:
  kind: value
  value: "-1"
  note: The object chooses its weapon as it would outside.
---

Counts from `0`: `0` is the primary weapon and `1` the secondary. While riding an [open-topped transport](/systems/transports/#firing-from-an-open-topped-transport), an object with both a primary and a secondary weapon fires this one at every target instead of choosing between them. A [`DeployFire=yes`](/keys/deployfire/#scope-infantrytype) soldier that has not deployed fires it too, whatever weapons it has.

```ini title="rulesmd.ini"
[MYSOLDIER] ; example InfantryType
Primary=MyRifle
Secondary=MyGrenade
OpenTransportWeapon=1 ; throws grenades from inside a transport
```
