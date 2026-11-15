---
key: NoManualEnter
scope: aircrafttype
summary: "With yes, a player's order cannot load a soldier or vehicle into this transport."
see_also: [NoManualUnload, "Passengers.Allowed", Passengers]
when_omitted:
  kind: value
  value: "no"
---

Over a `NoManualEnter=yes` transport, a selected soldier or vehicle shows the select cursor, and a click does not send it aboard. Scripts, triggers and computer players can still load the transport, and the key keeps nothing out. [`Passengers.Allowed`](/keys/passengers.allowed/) is the key that refuses passengers.

```ini title="rulesmd.ini"
[MYTANK] ; example VehicleType
Passengers=1
NoManualEnter=yes
```
