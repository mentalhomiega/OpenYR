---
title: Stop a Psychic Dominator blast from freezing the game over overlapping structures
category: fix
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

We fixed a permanent freeze after a Psychic Dominator blast destroyed two structures whose footprints share a cell, such as a map structure with another one placed over its edge. When the first structure left the map, the cell kept the second one as its occupier, and the next animation to look at the cell stopped the game for good once the second structure was deleted. A cell now drops a structure that does not stand on it when the structure before it is removed.
