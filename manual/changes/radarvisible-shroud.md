---
title: Keep radar-visible objects off the radar under shroud
category: fix
release: 0.2.0
targets:
- type: key
  id: RadarVisible
  effect: changed
credit:
- MentalHomiega
---

An object with [`RadarVisible=yes`](/keys/radarvisible/) is no longer plotted on the radar while its cell is under the player's shroud. Once the player reveals the cell, it is plotted as before, through fog of war and cloaking.
