---
title: Dock a ship at the dock point of a naval yard
category: fix
release: 0.2.0
targets:
- type: system
  id: repair
  effect: changed
credit:
- MentalHomiega
---

We now dock a vehicle at its repair building's dock point, which is the building's center plus the `DockingOffset0` from its art. A naval yard's dock lies beside its footprint, so a ship ordered to enter the yard docks there and is repaired. Before, we docked only a vehicle that stood on the building's center cell, which a naval yard's footprint blocks, so the ship stopped at the dock without docking. Depots without an offset dock at their center, as before.
