---
title: Stop a computer-controlled Yuri selling its base when the miner moves
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit: [MentalHomiega]
---

A computer player counts a slave miner that has packed up as a refinery when it checks whether it can still earn money. Before, a Yuri whose only refinery packed up to move to new ore sold its newest buildings to pay for another refinery.
