---
key: OpenTopped
summary: "Lets the passengers of this vehicle fire from inside it."
see_also: [OpenTransportWeapon, FireInTransport, "system:transports"]
when_omitted:
  kind: value
  value: "no"
---

The passengers of an open-topped vehicle stay at its position and fire their own weapons from inside it, and the vehicle passes its attack and stop orders on to them. Its own weapons reach no farther than the shortest primary weapon among its passengers. [Firing from an open-topped transport](/systems/transports/#firing-from-an-open-topped-transport) covers the rules.

```ini title="rulesmd.ini"
[MYFORTRESS] ; example VehicleType
Passengers=5
OpenTopped=yes
```
