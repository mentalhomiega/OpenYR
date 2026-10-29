---
title: Drop a passenger by weapon half a cell off the plane
category: fix
release: 0.2.0
targets:
- type: system
  id: aircraft-operations
  effect: changed
credit:
- MentalHomiega
---

A plane that drops a passenger by its weapon now places the passenger half a cell to one side of the plane, as an overfly paradrop already does. Before, the passenger landed on the plane's own cell. The drop needs a free landing spot in that cell; without one, the passenger stays aboard.
