---
title: Unload a harvester's whole load at once
category: fix
release: 0.2.0
targets:
- type: key
  id: HarvesterDumpRate
  effect: changed
- type: system
  id: tiberium
  effect: changed
- type: system
  id: veins
  effect: changed
credit: [MentalHomiega]
---

A docked harvester or weeder now hands over everything it holds of one Tiberium type in a single pass, and `HarvesterDumpRate` sets the time between passes. Before, it handed over one unit per pass, so a full War Miner took minutes to empty. The refinery also plays its `SpecialAnim` while the harvester unloads, and a harvester whose dock disappears mid-unload goes back to harvesting.
