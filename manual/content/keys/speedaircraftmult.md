---
key: SpeedAircraftMult
summary: A factor on a country's aircraft that changes no speed in the game.
no_effect: true
see_also: [SpeedInfantryMult, SpeedUnitsMult, Speed, "system:movement-and-terrain"]
when_omitted:
  kind: value
  value: "1.0"
---

An aircraft flies at its type's [`Speed=`](/keys/speed/#scope-aircrafttype) times the aircraft's own throttle, and nothing else changes that figure, so no value here changes how fast an aircraft flies. The key is read from the country section and stored.

```ini title="rulesmd.ini"
[MYCOUNTRY] ; example country
SpeedAircraftMult=1.15 ; example: no effect
```
