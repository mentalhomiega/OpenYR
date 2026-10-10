---
title: Base sale buys the item the rules name first
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

A computer house that sells its base for money now picks what to buy from its country's first listed harvester and its first buildable refinery or war factory. A country with no listed harvester no longer buys its refinery's slave miner in that choice. Inserting a refinery node after a base sale no longer removes the other refinery nodes.
