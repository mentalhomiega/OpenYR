---
title: Wake an idle repair or rearming structure for any docked object
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

An idle `UnitRepair=yes` or `UnitReload=yes` structure with several docks now leaves guard when any docked object needs it, not only the one on the first dock. A repair structure wakes for an object within a quarter of a cell of its center, and a rearming pad for an aircraft that needs ammunition. A repair structure still repairs only the object on its first dock. A structure with one dock behaves as before.
