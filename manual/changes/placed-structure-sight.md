---
title: Lift the shroud around a structure as it is placed
category: fix
release: 0.2.0
targets:
- type: system
  id: map-visibility
  effect: changed
credit: [MentalHomiega]
---

A structure now reveals the ground within its `Sight=` as soon as it is placed, even on ground the player had already discovered. Before, a structure placed next to the shroud revealed nothing, so the shroud stayed in place beside a new power plant or yard. Structures of every house sight on placement, as in gamemd.
