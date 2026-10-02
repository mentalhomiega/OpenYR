---
key: PoweredUnit
summary: "Shuts a unit down while its owner has no working control structure for it."
see_also: [PowersUnit]
when_omitted:
  kind: value
  value: "no"
---

A unit whose type sets `PoweredUnit=yes` works only while its owner has at least one structure whose [`PowersUnit`](/keys/powersunit/) names that type and which is working: powered, finished and not being sold. Without one the unit shuts down: it drops its target and orders and cannot move or fire. It starts again as soon as such a structure works. A unit standing in a structure's cell, such as one leaving a factory, does not shut down.

```ini title="rulesmd.ini"
[MYROBOT] ; example VehicleType
PoweredUnit=yes
```
