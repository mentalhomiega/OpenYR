---
title: Load a save made while a unit or rally point targets a spot off the map
category: fix
release: 0.2.0
targets:
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

A saved game now loads when a unit or structure holds a destination or rally point on a cell off the map. Before, such a save stopped the game with a message that pointers could not be remapped, so a skirmish saved after a computer unit was sent off the map could not be loaded.
