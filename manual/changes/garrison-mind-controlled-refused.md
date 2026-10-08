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

A soldier under mind control can no longer move into a structure. We refuse it as gamemd's BuildingClass::CanBeOccupiedBy does. Before, such a soldier could move in like any other.
