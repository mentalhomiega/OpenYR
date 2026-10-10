---
title: Base sale checks income by the rules gamemd uses
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

A computer house now decides whether it can make money from the refinery, gatherer, construction yard, base unit, war factory, and harvester it owns, compared with the costs of the items it needs next, in the order gamemd checks them. A country with no listed harvester no longer uses its refinery's slave miner in that decision.
