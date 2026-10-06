---
title: Resume a loaded game with the random numbers it was saved with
category: fix
release: 0.2.0
targets:
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

A loaded game now draws the same random numbers the saved game would have drawn next. Each house, vehicle and building used to take numbers from the game's generator as the load rebuilt it, which moved the generator past where the save left it.
