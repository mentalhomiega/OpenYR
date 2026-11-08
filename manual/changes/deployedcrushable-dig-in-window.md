---
title: Protect a DeployedCrushable=no soldier from the end of its dig-in to the end of its pack-up
category: fix
release: 0.2.0
targets:
- type: key
  id: DeployedCrushable
  effect: changed
credit:
- MentalHomiega
---

A [`DeployedCrushable=no`](/keys/deployedcrushable/#scope-infantrytype) soldier is now safe from crushing once its dig-in sequence has ended and stays safe until its pack-up sequence has ended, as in Yuri's Revenge. Before, it was safe from the start of the dig-in and could be crushed during the pack-up.
