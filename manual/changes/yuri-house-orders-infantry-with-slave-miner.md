---
title: Let a Yuri computer house order infantry while a slave miner is on order
category: fix
release: 0.2.0
targets:
- type: key
  id: AIAlternateProductionCreditCutoff
  effect: changed
- type: system
  id: ai-team-production
  effect: changed
credit:
- MentalHomiega
---

A computer house that has run short of credits and is ordering only vehicles, infantry and aircraft no longer treats a slave miner as the harvester that holds infantry and aircraft back. As in Yuri's Revenge, only the first [`HarvesterUnit`](/keys/harvesterunit/) type the house's country may own does that, so a Yuri house, which may own none, orders infantry and aircraft while a slave miner is on order. Before, such a house ordered nothing else until the slave miner was delivered.
