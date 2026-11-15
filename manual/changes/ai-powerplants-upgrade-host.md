---
title: Skip a listed power plant upgrade that has no free host
category: fix
release: 0.2.0
targets:
- type: key
  id: AI.PowerPlants
  effect: changed
credit: [MentalHomiega]
---

A computer house short of power no longer picks an upgrade from `AI.PowerPlants=` when none of its structures has room for it. Before, it started the upgrade, found no place for it and abandoned it, over and over, and built nothing else.
