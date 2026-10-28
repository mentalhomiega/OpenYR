---
title: Chronosphere units land where they are sent
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

A unit that a chronosphere warps now lands on the cell it was sent to even where it cannot stand, as in Yuri's Revenge. A vehicle set down on water it cannot cross sinks there, and a soldier set down on water or another cell it cannot enter is destroyed. Before, the unit was moved to the nearest cell it could stand on, which could be many cells from the target. A structure or tree on the target cell still moves the unit to a nearby cell.
