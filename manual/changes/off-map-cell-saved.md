---
title: Keep a destination outside the map across a save and load
category: fix
release: 0.2.0
targets:
- type: format
  id: save-games
  effect: changed
breaking: false
credit:
- MentalHomiega
---

A unit whose destination or target is a cell outside the map now heads for the same place after a save and load. Every cell outside the map shares one stand-in cell, and a load put that cell at cell 0,0 instead of where the game had left it. Save games now store its location, so a save from an earlier build is refused.
