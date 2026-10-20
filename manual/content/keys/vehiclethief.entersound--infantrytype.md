---
key: VehicleThief.EnterSound
scope: infantrytype
summary: "The sound a soldier plays where it takes over a vehicle."
see_also: [VehicleThief, CanDrive]
when_omitted:
  kind: value
  value: "none, so the capture is silent"
---

A sound name. A soldier with [`VehicleThief=yes`](/keys/vehiclethief/) or [`CanDrive=yes`](/keys/candrive/) plays it at the vehicle when it enters the vehicle on a capture order. A vehicle that runs over a thief heading for it, and a soldier that takes a structure, play nothing.

```ini title="rulesmd.ini"
[HIJACKER] ; example InfantryType
VehicleThief=yes
VehicleThief.EnterSound=VehicleHijack
```
