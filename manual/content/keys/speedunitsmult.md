---
key: SpeedUnitsMult
summary: The factor on the top speed of a house of this country's vehicles.
see_also: [SpeedInfantryMult, SpeedAircraftMult, Speed, "system:movement-and-terrain"]
when_omitted:
  kind: value
  value: "1.0"
---

A vehicle of a house of this country has its top speed multiplied by this value, so a value above 1 moves it faster. The top speed is the one [`Speed=`](/keys/speed/#scope-aircrafttype) gives. The value applies in every game type to vehicles on the Drive, Hover, Walk, Mech and Tunnel locomotors, and the speed is rounded down to a whole number after the multiply. A vehicle on any other locomotor does not use it. [Why it is slower than its Speed says](/systems/movement-and-terrain/#why-it-is-slower-than-its-speed-says) places it among the other speed factors.

```ini title="rulesmd.ini"
[MYCOUNTRY] ; example country
SpeedUnitsMult=1.15 ; example: this country's vehicles move 15% faster
```
