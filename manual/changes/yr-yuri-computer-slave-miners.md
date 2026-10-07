---
title: Build slave miners for a computer-controlled Yuri
category: fix
release: 0.2.0
targets:
- type: system
  id: slave-miners
  effect: changed
- type: key
  id: AISlaveMinerNumber
  effect: changed
- type: key
  id: HarvesterUnit
  effect: changed
credit: [MentalHomiega]
---

A computer player whose country may own none of the `HarvesterUnit=` units, such as Yuri, now builds the slave miner its refinery packs up into (`UndeploysInto=`), until it has `AISlaveMinerNumber` ore gatherers. Its ability-to-earn checks count a slave miner and the refinery it has deployed into as gatherers, and its replacement miner is priced at the slave miner's cost. Before, the first listed harvester was built for it. Yuri could not use that War Miner, and the computer then had no income.
