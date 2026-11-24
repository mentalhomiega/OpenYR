---
title: Send slaves back to the deployed miner's dock cell
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit: [MentalHomiega]
---

Slaves returning to a deployed miner walk to its dock cell and unload once they stand on it with no destination left, as in Yuri's Revenge. A slave in its miner's dock cell is no longer scattered out of it while guarding, and it takes the spot nearest its coordinate there even when another slave holds that spot. Before, slaves unloaded from within one or two cells of the dock.
