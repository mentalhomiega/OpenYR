---
title: Stop a warping-out structure from giving or drawing power
category: fix
release: 0.2.0
targets:
- type: system
  id: temporal-weapons
  effect: changed
credit:
- MentalHomiega
---

A structure that is being warped out, by a temporal erase or a chronosphere warp, gives no power and draws none while the warp runs. The house counts the change the next time it recounts its power.

The warp does not disable the structure's superweapon. A chronosphere being erased keeps its superweapon available, as gamemd's superweapon grant and launch checks do, unless the house falls short of power.
