---
title: Stop a slave miner circling deploy spots it cannot use
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit: [MentalHomiega]
---

A slave miner now picks a deploy spot on level ground, and a computer player's miner tells friendly units to leave the footprint when its structure does not fit. A miner sent to new ore also no longer drives back to the place it stopped at before. Before, a miner could choose a spot on a slope, find it could not unpack there, and hop between two such spots for the rest of the game, which left a computer-controlled Yuri without income. A miner also set off for new ore and then returned to its earlier stop on arrival, so it never unpacked at the ore.
