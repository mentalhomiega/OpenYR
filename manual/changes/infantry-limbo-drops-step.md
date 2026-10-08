---
title: Stop an infantryman's stale step from blocking a cell
category: fix
release: 0.2.0
targets:
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

An infantryman that enters a building no longer keeps the step it was walking when it comes back out. Before, it resumed that step and left a place in the cell claimed that no infantryman stood on. A vehicle that needed the cell waited for the claim to clear, which never happened, so a computer-controlled Yuri's slave miner stalled for over 4000 frames in a skirmish soak.
