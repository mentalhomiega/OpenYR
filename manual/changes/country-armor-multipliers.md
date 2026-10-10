---
title: Divide damage by the country's armor multiplier for the object's kind
category: fix
release: 0.2.0
targets:
- type: key
  id: ArmorInfantryMult
  effect: added
- type: key
  id: ArmorUnitsMult
  effect: added
- type: key
  id: ArmorAircraftMult
  effect: added
- type: key
  id: ArmorBuildingsMult
  effect: added
- type: key
  id: ArmorDefensesMult
  effect: added
- type: key
  id: Armor
  scope: housetype
  effect: changed
- type: system
  id: warheads
  effect: changed
credit:
- MentalHomiega
---

A country's `Armor=` used to divide the damage all of its objects took. Yuri's Revenge ignores it and divides by `ArmorInfantryMult`, `ArmorUnitsMult`, `ArmorAircraftMult`, `ArmorBuildingsMult` or `ArmorDefensesMult`, by the kind of object hit, so we now do the same. `Armor=` is still read and has no effect. The stock rules set none of the five keys, so stock games are unchanged.
