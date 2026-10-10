---
title: Base sale checks the item the house needs next
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

A computer house that cannot make money now plans around a war factory when no refinery can be built, and sells its base when neither a refinery nor a war factory can be built. Before this change it looked only at refineries. The check for a refinery under construction now ignores the owner of the building, as gamemd's check does.
