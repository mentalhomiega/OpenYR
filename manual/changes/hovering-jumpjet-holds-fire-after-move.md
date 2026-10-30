---
title: Hold fire when a hovering jumpjet finishes a move
category: fix
release: 0.2.0
targets:
- type: key
  id: BalloonHover
  effect: changed
credit: [MentalHomiega]
---

A Kirov, Siege Chopper or Rocketeer that flies to a cell and stops there stays on its move order and holds fire until it is given a new order, as in Yuri's Revenge. Before, it went on guard and shot at anything that came into range. An attack order, or a Guard or Stop command, still makes it fire.
