---
title: Deploy small structures on the vehicle's own cell
category: fix
release: 0.2.0
targets:
- type: key
  id: DeploysInto
  effect: changed
- type: key
  id: UndeploysInto
  effect: changed
credit: [MentalHomiega]
---

A vehicle that deploys into a structure no larger than 2x2, such as the Slave Miner, now places the structure's top-left cell on its own cell, and comes back on that cell when the structure packs up, as in Yuri's Revenge. Before, the structure was placed one cell north-west of the vehicle and the vehicle came back one cell south-east of the corner. A slave miner that drives to ore by itself still deploys on the same cells, since it now stops one cell further north-west.
