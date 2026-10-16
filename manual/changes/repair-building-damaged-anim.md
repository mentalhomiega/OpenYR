---
title: Show a repair building's damaged repair animation
category: fix
release: 0.2.0
targets:
- type: key
  id: SpecialAnimThreeDamaged
  effect: changed
credit:
- MentalHomiega
---

A `UnitRepair=yes` building, such as the Service Depot, now plays its `SpecialAnimThree=` animation in the damaged form, `SpecialAnimThreeDamaged=`, when its health is at or below `ConditionYellow`. Above it, the healthy form plays as before. Before, the healthy form played at every health.
