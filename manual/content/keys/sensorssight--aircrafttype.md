---
key: SensorsSight
scope: aircrafttype
label: 'Mobile sensor radius'
see_also: [Sensors, SensorArray, "system:cloaking"]
when_omitted:
  kind: value
  value: "0"
---

A vehicle, infantryman or aircraft with [`Sensors=yes`](/keys/sensors/) marks as sensed, for its owner, every cell within this many cells of the cell it stands on. Its owner can then see and target cloaked objects in those cells, such as submarines. The coverage moves with the object; see [Mobile sensors](/systems/cloaking/#mobile-sensors). With `0`, or without `Sensors=yes`, the object marks nothing as sensed.

```ini title="rulesmd.ini"
[MYDESTROYER] ; example VehicleType
Sensors=yes
SensorsSight=8
```
