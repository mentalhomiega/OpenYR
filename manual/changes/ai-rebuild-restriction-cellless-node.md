---
title: Hold back a lost computer structure whose base node has no cell
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

A computer base node that has held a structure but has since lost its cell is now held back after an attack like any other node that has held one. We used to fill it at once, because a node without a cell was treated as one that had never held a structure.
