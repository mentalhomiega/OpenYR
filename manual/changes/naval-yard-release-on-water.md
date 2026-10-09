---
title: Release a ship from a naval yard onto water
category: fix
release: 0.2.0
targets:
- type: key
  id: Naval
  effect: changed
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

We now release a ship from a `Naval=yes` structure with `WeaponsFactory=yes` onto a water cell. The ship goes to the water cell out of the yard in the direction of its rally point when that cell is free, and otherwise to the water cell nearest the yard. Before, we placed the ship at the yard's exit coordinate, which can lie on land or inside the footprint. A ship released from a yard on the shore stayed on land, with its move order unable to finish.
