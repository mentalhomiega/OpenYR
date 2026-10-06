---
title: Wake an idle repair bay for any docked object
category: fix
release: 0.2.0
targets:
- type: key
  id: UnitRepair
  effect: changed
- type: key
  id: UnitReload
  effect: changed
credit: [MentalHomiega]
---

An idle `UnitRepair=yes` structure with several docks now starts repairing when any docked object comes within a quarter of a cell of the center, and a pad that also sets `UnitReload=yes` wakes for any docked aircraft that needs rearming. Before, only the object on the first dock could wake them. A structure with one dock behaves as before.
