---
title: Hold fire when a hovering jumpjet vehicle finishes a move
category: fix
release: 0.2.0
targets:
- type: key
  id: BalloonHover
  effect: changed
credit: [MentalHomiega]
---

A Kirov or Siege Chopper that flies to a cell and stops there stays on its move order and holds fire until it is given a new order. An attack order, or a Guard or Stop command, still makes it fire. A Rocketeer that has stopped after a move order attacks enemies that come into range on its own.
