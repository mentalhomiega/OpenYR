---
title: Hold move orders while a Siege Chopper deploys
category: fix
release: 0.2.0
targets:
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

A simple deployer such as the Soviet Siege Chopper now takes no move order while its deploy or pack-up animation runs. Before, a move order cut the animation short: the unit stopped deploying and drove off. The unit now finishes the animation, stays where it is, and drops the held order.
