---
title: Skip power plants during a blackout or drain
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

A computer house no longer inserts a power plant while it is in a power blackout or while an enemy drains one of its power plants. Before, it inserted a plant whenever its drain exceeded its output, even during a blackout that stopped its plants from making power.
