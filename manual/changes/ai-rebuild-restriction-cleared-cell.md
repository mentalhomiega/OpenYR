---
title: Hold back the rebuild of a computer structure whose node has no cell
category: fix
release: 0.2.0
targets:
- type: key
  id: AIRestrictReplaceTime
  effect: changed
- type: system
  id: ai-base-building
  effect: changed
credit: [MentalHomiega]
---

A computer house's base node whose cell was cleared to `0,0` is now held back after an attack like any other node that has held a structure. Before, such a node was rebuilt at once, because the check treated cell `0,0` as a node that had never held a structure.
