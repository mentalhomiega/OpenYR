---
title: Keep computer harvesters at HarvestersPerRefinery per refinery
category: fix
release: 0.2.0
targets:
- type: key
  id: HarvestersPerRefinery
  effect: added
- type: key
  id: AISlaveMinerNumber
  effect: changed
- type: system
  id: tiberium
  effect: changed
credit: [MentalHomiega]
---

A computer house now keeps `HarvestersPerRefinery` ore gatherers for each refinery it owns, read from `[General]` at its difficulty slot, and falls back to `AISlaveMinerNumber` when no refinery type is buildable. Before, the count was hard-coded at two harvesters per refinery, one at slot 2, and one in every campaign, and the rule was never read. The shipped rules keep the same counts outside a campaign.
