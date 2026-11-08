---
title: Print control group numbers beside the health bar
category: fix
release: 0.2.0
targets:
- type: key
  id: UnitGroupNumberOffset
  effect: changed
- type: key
  id: InfantryGroupNumberOffset
  effect: changed
- type: key
  id: BuildingGroupNumberOffset
  effect: changed
- type: key
  id: AircraftGroupNumberOffset
  effect: changed
credit:
- MentalHomiega
---

A selected object's control group number now appears in a black box edged in its owner's color, beside the left end of its health bar, as in Yuri's Revenge. Before, it was plain text below the object. The defaults of the eight `GroupNumberOffset` keys are `-4,-36`, and `-4,-39` for infantry, with or without pips.
