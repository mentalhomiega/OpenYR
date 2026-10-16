---
title: Refuse mind-controlled soldiers at a garrison
category: fix
release: 0.2.0
targets:
- type: system
  id: garrisons
  effect: changed
credit: [MentalHomiega]
---

A soldier with `Occupier=yes` that is under mind control can no longer garrison a structure, as gamemd's BuildingClass::CanBeOccupiedBy decides. Before, such a soldier could move in like any other.
