---
key: VehicleThief.OneTime
scope: infantrytype
summary: "With yes, a vehicle thief is used up by the theft and does not step out when the vehicle dies."
see_also: [VehicleThief, VehicleThief.LeaveSound, "system:capture"]
when_omitted:
  kind: value
  value: "no"
---

`VehicleThief.OneTime` is an Ares key for a [`VehicleThief=yes`](/keys/vehiclethief/) soldier. With `yes`, the soldier is removed when it takes a vehicle, and the vehicle does not record its type. When the vehicle is destroyed, no thief steps out of the wreck; the vehicle's own crew escape roll applies as for any other vehicle of its new owner. A vehicle stolen this way does not count against the thief type's [`BuildLimit`](/keys/buildlimit/).

With `no`, a soldier of the thief's type steps out of the wreck, as [Stealing a vehicle](/systems/capture/#stealing-a-vehicle) describes.

```ini title="rulesmd.ini"
[HIJACKER] ; example InfantryType
VehicleThief=yes
VehicleThief.OneTime=yes
```
