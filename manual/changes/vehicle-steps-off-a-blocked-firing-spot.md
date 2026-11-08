---
title: Step off a blocked firing spot when a vehicle approaches its target
category: fix
release: 0.2.0
targets:
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

A vehicle sent at a target looks for a spot from which its weapon reaches. A spot it cannot enter, for example a cell holding a structure, is no longer dropped. As in Yuri's Revenge, the vehicle steps off it, adjacent cell after adjacent cell, and takes the first cell its weapon reaches from that it can enter. Before, it went on to a spot farther from the target.
