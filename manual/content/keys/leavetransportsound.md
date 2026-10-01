---
key: LeaveTransportSound
summary: "The sound played at a transport of this type when a passenger leaves it."
see_also: [EnterTransportSound]
when_omitted:
  kind: value
  value: none
---

When a transport vehicle of this type unloads a passenger, this sound plays at the transport, once for each passenger.

```ini title="rulesmd.ini"
[MYAPC] ; example VehicleType
LeaveTransportSound=ExitTransport
```
