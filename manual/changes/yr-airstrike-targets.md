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

A unit with an airstrike second weapon, such as Boris, now uses its first weapon on anything that is not a structure that allows C4, and on a structure that is both a `ResourceGatherer` and a `ResourceDestination`, such as a deployed Slave Miner, as in Yuri's Revenge.
