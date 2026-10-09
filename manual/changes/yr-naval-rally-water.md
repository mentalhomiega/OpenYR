---
title: Give shipyards a water rally point
category: fix
release: 0.2.0
targets:
- type: system
  id: production
  effect: changed
credit: [MentalHomiega]
---

A `Naval=yes` structure now searches for its rally point among cells an amphibious unit can enter, so a click on water sets a rally point that a ship can reach. Before, the search used foot-passable cells, so a ship ordered to a water rally point stopped beside the structure instead.
