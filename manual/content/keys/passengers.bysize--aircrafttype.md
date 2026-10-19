---
key: Passengers.BySize
scope: aircrafttype
summary: "With no, every passenger takes one place of Passengers, whatever its Size."
see_also: [Passengers, Size, SizeLimit, "Passengers.Allowed"]
when_omitted:
  kind: value
  value: "yes, so a passenger takes as many places as its Size"
---

With `Passengers.BySize=no`, the transport holds as many passengers as `Passengers=` says, each taking one place. [`SizeLimit=`](/keys/sizelimit/) still keeps out a passenger whose `Size=` is above it. Only the check at loading changes: the places shown and a transport's other uses of its load still count by `Size`.

```ini title="rulesmd.ini"
[MYTRUCK] ; example VehicleType
Passengers=5
SizeLimit=2
Passengers.BySize=no
```
