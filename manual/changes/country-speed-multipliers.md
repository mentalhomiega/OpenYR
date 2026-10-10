---
title: Scale object speed by the country's speed multiplier
category: fix
release: 0.2.0
targets:
- type: key
  id: SpeedInfantryMult
  effect: added
- type: key
  id: SpeedUnitsMult
  effect: added
- type: key
  id: SpeedAircraftMult
  effect: added
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

Yuri's Revenge scales the top speed of a country's soldiers and vehicles by `SpeedInfantryMult` and `SpeedUnitsMult`, so we now do the same. A flying aircraft takes no speed from a country, so `SpeedAircraftMult` is read and changes nothing. The stock rules set none of the three keys, so stock games are unchanged.
