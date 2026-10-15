---
title: Repair bays serve only the object on their first dock
category: fix
release: 0.2.0
targets:
- type: key
  id: NumberOfDocks
  effect: changed
credit:
- MentalHomiega
---

A `UnitRepair=yes` structure now starts repairing the object on its first dock once the object stops within 100 leptons of the dock's spot, or 200 for a hovercraft or ship, as the original game does. Before, the start check used 64 leptons from the dock (150 for a hovercraft).
