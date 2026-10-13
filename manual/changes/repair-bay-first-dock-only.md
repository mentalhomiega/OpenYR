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

A `UnitRepair=yes` structure now repairs only the object on its first dock, as the original game does. It starts on that object once the object stops within 100 leptons of the dock's spot, or 200 for a hovercraft or ship. Before, every docked object was repaired, and the start check used 64 leptons from the dock (150 for a hovercraft).
