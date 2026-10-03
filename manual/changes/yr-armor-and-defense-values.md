---
title: Use Yuri's Revenge's armor classes and defense values
category: feature
release: 0.2.0
targets:
- type: enum
  id: ArmorType
  effect: changed
- type: key
  id: Verses
  effect: changed
- type: key
  id: AntiAirValue
  effect: added
- type: key
  id: AntiArmorValue
  effect: added
- type: key
  id: AntiInfantryValue
  effect: added
credit: [MentalHomiega]
---

Armor now has Yuri's Revenge's eleven classes, `none` through `special_2`, and a warhead's `Verses` lists eleven entries in that order. A rules file written for five classes must be rewritten: its armor names and `Verses` lists no longer line up.

A BuildingType's anti-air, anti-armor and anti-infantry defense values are now read from `AntiAirValue`, `AntiArmorValue` and `AntiInfantryValue` instead of being worked out from its weapon, so `MaximumBaseDefenseValue` no longer has any effect.
