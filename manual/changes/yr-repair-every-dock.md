---
title: Repair every object docked at a repair pad
category: feature
release: 0.2.0
targets:
- type: key
  id: UnitRepair
  effect: changed
credit: [MentalHomiega]
---

A `UnitRepair=yes` structure with several docks now repairs the object on each dock. Before, it repaired only the object on the first dock. A structure with one dock behaves as before.
