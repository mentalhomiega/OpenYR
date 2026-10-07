---
title: Let slaves start shoveling as soon as they stand on ore
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit: [MentalHomiega]
---

A slave of a deployed slave miner now starts shoveling when it stands on ore, even if a move order is still pending, as in Yuri's Revenge. Before, it waited for the order to clear. A slave that had been idle on the Guard Area mission was sent back to where it had stood, so on some maps it walked between its ore and that spot and never mined. A computer-controlled Yuri on such a map earned nothing.
