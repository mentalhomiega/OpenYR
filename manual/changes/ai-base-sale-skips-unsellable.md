---
title: Base sale keeps structures that cannot be sold
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
- type: key
  id: Unsellable
  effect: changed
credit:
- MentalHomiega
---

A computer house that sells its base for money no longer sells a structure that cannot be sold. It removes that structure's node and keeps the structure standing, with no refund. Before this change the sale took the structure and paid its refund.
