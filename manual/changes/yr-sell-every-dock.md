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

Selling a `UnitRepair=yes` structure with several docks now sells each object parked on a dock, not only the one on the first dock. When the structure itself is sold, every docked object is told to move away. A structure with one dock behaves as before.
