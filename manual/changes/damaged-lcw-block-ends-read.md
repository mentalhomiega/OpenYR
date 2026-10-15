---
title: End a map's LCW read at a damaged block
category: fix
release: 0.2.0
targets:
- type: format
  id: scenario-terrain
  effect: changed
credit:
- MentalHomiega
---

A map whose compressed terrain or overlay data holds a damaged block now stops reading at that block. Before, such a block could read past its data and crash the game, or fill cells with stray values. An overlay number for which the rules define no overlay type now leaves its cell without an overlay.
