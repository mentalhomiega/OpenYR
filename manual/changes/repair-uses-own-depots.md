---
title: Repair missions use only the owner's depots
category: fix
release: 0.2.0
targets:
- type: system
  id: repair
  effect: changed
credit:
- MentalHomiega
---

A vehicle sent on a repair mission now chooses only among its owner's repair depots, as in Yuri's Revenge. Before, it could also choose a nearer allied player's depot, but only while its owner had at least one depot of that type. Sending a unit into an allied depot by hand still works.
