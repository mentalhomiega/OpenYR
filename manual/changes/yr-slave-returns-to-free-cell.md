---
title: Send slaves back to a free cell beside the deployed miner
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit: [MentalHomiega]
---

Slaves returning to a deployed miner now walk to the free cell nearest its dock and unload once within one cell of the dock, or within two cells if they have stopped. Before, they were sent to the dock cell inside the structure and could stall short of it when the cells around it were crowded, so a computer player's miner delivered no ore for long stretches.
