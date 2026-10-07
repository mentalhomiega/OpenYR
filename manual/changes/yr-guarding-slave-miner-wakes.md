---
title: Wake a slave miner that guards an area
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
- type: key
  id: SlaveMinerKickFrameDelay
  effect: changed
credit: [MentalHomiega]
---

A mobile slave miner on the Area Guard mission now looks for ore after `SlaveMinerKickFrameDelay` frames and drives off to deploy beside it, as it already did on the Guard mission. Before, a miner a computer player built stood where it came out of the war factory for the rest of the game.
