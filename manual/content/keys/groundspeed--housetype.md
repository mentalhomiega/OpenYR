---
key: Groundspeed
scope: housetype
label: Country speed multiplier
see_also: ["system:difficulty"]
when_omitted:
  kind: value
  value: "1.0"
---

A house of this country combines this multiplier with [the difficulty section's `Groundspeed=`](/keys/groundspeed/#scope-difficulty-settings) and [`GameSpeedBias`](/keys/gamespeedbias/) [when it is given its difficulty slot](/systems/difficulty/#how-the-figures-are-combined). A campaign game leaves the country's value out, as it does for [`Airspeed=`](/keys/airspeed/#scope-housetype).

Nothing in the game reads the combined figure, so no value here changes how fast anything moves. Vehicles and soldiers move at the speed [`Speed=`](/keys/speed/#scope-aircrafttype) gives their type, scaled by the factors in [Why it is slower than its Speed says](/systems/movement-and-terrain/#why-it-is-slower-than-its-speed-says), including the country's [`SpeedInfantryMult`](/keys/speedinfantrymult/) and [`SpeedUnitsMult`](/keys/speedunitsmult/).
