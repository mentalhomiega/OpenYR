---
key: VehicleThief.LeaveSound
scope: infantrytype
summary: "The sound a soldier plays where it steps out of the wreck of a vehicle it took."
see_also: [VehicleThief, VehicleThief.EnterSound, VehicleThief.OneTime]
when_omitted:
  kind: value
  value: "none"
---

`VehicleThief.LeaveSound` is an Ares key. A sound name. When a vehicle taken by a soldier of this type is destroyed and the soldier steps out of the wreck, the sound plays where the soldier appears. It also plays for a [`CanDrive=yes`](/keys/candrive/) soldier that steps out of a vehicle it drove. Nothing plays when the soldier finds no room to stand, or when the thief was used up by [`VehicleThief.OneTime=yes`](/keys/vehiclethief.onetime/).

```ini title="rulesmd.ini"
[HIJACKER] ; example InfantryType
VehicleThief=yes
VehicleThief.LeaveSound=VehicleHijackLeave
```
