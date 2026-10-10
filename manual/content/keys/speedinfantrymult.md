---
key: SpeedInfantryMult
summary: The factor on the top speed of a house of this country's infantry.
see_also: [SpeedUnitsMult, SpeedAircraftMult, Speed, "system:movement-and-terrain"]
when_omitted:
  kind: value
  value: "1.0"
---

An infantryman of a house of this country has its top speed multiplied by this value, so a value above 1 moves it faster. The top speed is the one [`Speed=`](/keys/speed/#scope-aircrafttype) gives. The value applies in every game type, and the speed is rounded down to a whole number after the multiply. [Why it is slower than its Speed says](/systems/movement-and-terrain/#why-it-is-slower-than-its-speed-says) places it among the other speed factors.

```ini title="rulesmd.ini"
[MYCOUNTRY] ; example country
SpeedInfantryMult=1.15 ; example: this country's infantry move 15% faster
```
