---
title: Repair only the first dock of a repair pad
category: fix
release: 0.2.0
targets:
- type: key
  id: UnitRepair
  effect: changed
credit: [MentalHomiega]
---

A `UnitRepair=yes` structure with several docks takes in one object per dock but repairs only the object on its first dock, as the original game does. An object on a later dock is not repaired.
