---
title: Sell every object parked at a repair bay
category: fix
release: 0.2.0
targets:
- type: key
  id: UnitRepair
  effect: changed
credit: [MentalHomiega]
---

Selling a `UnitRepair=yes` structure with several docks now sells each object parked within half a cell of its center, not only the one on the first dock. When no object is that close, the structure itself is sold and every docked object is told to move away. A structure with one dock behaves as before.
