---
key: EnterTransportSound
summary: "The sound played at a transport of this type when a passenger boards it."
see_also: [LeaveTransportSound]
when_omitted:
  kind: value
  value: none
---

When a soldier or vehicle boards a transport vehicle of this type, this sound plays at the transport.

```ini title="rulesmd.ini"
[MYAPC] ; example VehicleType
EnterTransportSound=EnterTransport
```
