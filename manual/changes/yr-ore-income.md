---
title: Pay for delivered ore at once
category: fix
release: 0.2.0
targets:
- type: key
  id: IncomeMult
  effect: added
- type: key
  id: Storage
  effect: changed
- type: key
  id: FillSilos
  effect: changed
credit: [Lucas]
---

Every house now receives credits for delivered ore as soon as a harvester unloads, scaled by its country's `IncomeMult`, as in Yuri's Revenge. Storage capacity no longer caps a player's income, so a player's credits keep rising after the refinery would have been full.
