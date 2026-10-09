---
title: Send every harvester to the richest nearby patch
category: fix
release: 0.2.0
targets:
- type: system
  id: tiberium
  effect: changed
- type: key
  id: HarvesterUnit
  effect: changed
credit: [MentalHomiega]
---

A computer-controlled harvester in a skirmish or multiplayer game no longer picks a patch at random, weighted by value and by the number of harvesters its house owns. It takes the richest cell of the nearest ring that has Tiberium, as every other harvester does in Yuri's Revenge. [`HarvesterUnit`](/keys/harvesterunit/) no longer affects where a computer house's harvesters search.
