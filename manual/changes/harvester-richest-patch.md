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

We removed the weighted search that computer harvesters used in skirmish and multiplayer games. Every harvester now takes the richest cell of the nearest ring that has Tiberium, as in Yuri's Revenge. [`HarvesterUnit`](/keys/harvesterunit/) no longer affects where a computer house's harvesters search.
