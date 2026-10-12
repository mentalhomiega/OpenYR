---
title: Start a slave miner's deploy as soon as it stops
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
credit: [MentalHomiega]
---

A slave miner that stops on its deploy spot now starts deploying at once when the structure fits. Before, the order waited for the miner's next turn, and a team's Area Guard order could replace it first. A miner on a team that held Area Guard, such as the slave miner teams in SOV05, then kept cycling between the two and did not deploy.
