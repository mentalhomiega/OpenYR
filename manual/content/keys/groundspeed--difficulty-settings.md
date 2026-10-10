---
key: Groundspeed
scope: difficulty-settings
label: Difficulty speed multiplier
see_also: ["system:difficulty"]
when_omitted:
  kind: value
  value: "1"
  note: The difficulty block is re-read from fixed defaults whenever its section is present, so a later file that contains the section without this key restores 1 rather than keeping the earlier value.
---

`[Easy]`, `[Normal]` and `[Difficult]` each set their own multiplier. A house combines the one for [its difficulty slot](/systems/difficulty/#from-the-setting-to-a-slot) with [`GameSpeedBias`](/keys/gamespeedbias/) and, outside a campaign game, with [its country's `Groundspeed=`](/keys/groundspeed/#scope-housetype).

Nothing in the game reads the combined figure, so no value here changes how fast anything moves. Vehicles and soldiers move at the speed [`Speed=`](/keys/speed/#scope-aircrafttype) gives their type, scaled by the factors in [Why it is slower than its Speed says](/systems/movement-and-terrain/#why-it-is-slower-than-its-speed-says), including the country's [`SpeedInfantryMult`](/keys/speedinfantrymult/) and [`SpeedUnitsMult`](/keys/speedunitsmult/).
