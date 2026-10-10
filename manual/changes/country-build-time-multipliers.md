---
title: Scale build time by the country's multiplier for the object's kind
category: fix
release: 0.2.0
targets:
- type: key
  id: BuildTimeInfantryMult
  effect: added
- type: key
  id: BuildTimeUnitsMult
  effect: added
- type: key
  id: BuildTimeAircraftMult
  effect: added
- type: key
  id: BuildTimeBuildingsMult
  effect: added
- type: key
  id: BuildTimeDefensesMult
  effect: added
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

Yuri's Revenge multiplies the build time of each object by its country's `BuildTimeInfantryMult`, `BuildTimeUnitsMult`, `BuildTimeAircraftMult`, `BuildTimeBuildingsMult` or `BuildTimeDefensesMult`, by the kind of object, so we now do the same. The stock rules set none of the five keys, so stock games are unchanged.
