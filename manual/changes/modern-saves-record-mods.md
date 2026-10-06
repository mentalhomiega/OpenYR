---
title: Record the active mods in saved games
category: feature
release: 0.2.0
targets:
- type: format
  id: save-games
  effect: changed
credit:
- MentalHomiega
---

A saved game now records the folder names of the active mods. Loading a save under a different list of mods still loads it, and then shows both lists in the message list and the debug log. Saves from earlier builds load as before.
