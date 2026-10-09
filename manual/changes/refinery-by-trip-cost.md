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

A loaded harvester reserves the nearest free refinery of any type in its [`Dock`](/keys/dock/) list only when that refinery is within [`HarvesterTooFarDistance`](/keys/harvestertoofardistance/) cells, measured in a straight line. A Chrono Miner uses [`ChronoHarvTooFarDistance`](/keys/chronoharvtoofardistance/) instead. Otherwise it drives to the nearest refinery of any kind and waits there. The wait is no longer estimated from the loads already waiting in line, so the rule no longer depends on `HarvesterDumpRate` or the harvesters' speed.
