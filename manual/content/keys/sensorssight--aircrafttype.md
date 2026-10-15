---
key: SensorsSight
scope: aircrafttype
label: 'Mobile sensor radius'
see_also: [Sensors, SensorArray, "system:cloaking"]
when_omitted:
  kind: value
  value: "0"
---

A vehicle, infantryman or aircraft with [`Sensors=yes`](/keys/sensors/) marks as sensed, for its owner, every cell whose distance from the cell it stands on is less than this many cells. A cell exactly this many cells away is not covered, so `SensorsSight=2` covers the 3-by-3 block around the object and not the cells two out in a straight line. Its owner can then see and target cloaked objects in those cells, such as submarines. The coverage moves with the object; see [Mobile sensors](/systems/cloaking/#mobile-sensors). With `0`, or without `Sensors=yes`, the object marks nothing as sensed.

```ini title="rulesmd.ini"
[MYDESTROYER] ; example VehicleType
Sensors=yes
SensorsSight=8
```
