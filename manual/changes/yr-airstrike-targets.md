---
title: Keep airstrikes off deployed Slave Miners
category: fix
release: 0.2.0
targets:
- type: key
  id: Airstrike
  effect: changed
- type: key
  id: ResourceGatherer
  effect: added
- type: key
  id: ResourceDestination
  effect: added
credit: [MentalHomiega]
---

A unit with an airstrike second weapon, such as Boris, now uses its first weapon on a deployed Slave Miner and on anything that is not a structure, as in Yuri's Revenge.
