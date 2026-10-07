---
title: Deploy a slave miner on ground that is free of ore
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit: [MentalHomiega]
---

A slave miner that drives to new ore now picks a deploy spot whose whole footprint is free of ore and other overlays. Before, it could stop on ore, find it could not unpack there, and try again from the same place, hopping five to eight times before it deployed.
