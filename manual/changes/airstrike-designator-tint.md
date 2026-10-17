---
title: Tint a structure that an airstrike designator marks
category: feature
release: 0.2.0
targets:
- type: key
  id: Airstrike
  effect: changed
credit: [MentalHomiega]
---

A structure that an airstrike's planes attack is drawn with the `LaserTargetColor` entry of the `[ColorAdd]` table added to its colours, as in Yuri's Revenge. The default entry adds nothing, so structures look as before until `LaserTargetColor=` names another entry. The tint lifts when the firer changes its target or either object leaves the map.
