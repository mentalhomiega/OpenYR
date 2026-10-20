---
key: CanBeDriven
scope: aircrafttype
summary: "Whether a CanDrive soldier can take over vehicles of this type."
see_also: [CanDrive, "VehicleThief.Allowed"]
when_omitted:
  kind: value
  value: "yes"
---

With `CanBeDriven=no`, soldiers with [`CanDrive=yes`](/keys/candrive/) cannot take over a vehicle of this type, even from a house that can be driven from. The key does not affect [`VehicleThief=yes`](/keys/vehiclethief/) soldiers, which [`VehicleThief.Allowed`](/keys/vehiclethief.allowed/) governs.

```ini title="rulesmd.ini"
[APOC] ; example VehicleType
CanBeDriven=no
```
