---
title: Reserve a refinery only when it is within reach
category: fix
release: 0.2.0
targets:
- type: key
  id: HarvesterTooFarDistance
  effect: added
- type: key
  id: ChronoHarvTooFarDistance
  effect: added
- type: key
  id: Dock
  effect: changed
- type: system
  id: tiberium
  effect: changed
credit: [ZivDero, Rampastring]
---

A loaded harvester still heads for the nearest refinery of any type in its [`Dock`](/keys/dock/) list. We now reserve the nearest free refinery only when it is within [`HarvesterTooFarDistance`](/keys/harvestertoofardistance/) cells, measured in a straight line. A Chrono Miner uses [`ChronoHarvTooFarDistance`](/keys/chronoharvtoofardistance/) instead. Otherwise the harvester drives to the nearest refinery of any kind and waits there, as in Yuri's Revenge. We removed the wait estimate, which counted the loads already in line, so the rule no longer depends on [`HarvesterDumpRate`](/keys/harvesterdumprate/) or on the harvesters' speed.
