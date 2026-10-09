---
title: Drop a computer base node after repeated blocked placements
category: fix
release: 0.2.0
targets:
- type: key
  id: MaximumBuildingPlacementFailures
  effect: added
- type: system
  id: ai-base-building
  effect: changed
credit:
- MentalHomiega
---

A computer house now counts each blocked placement of a structure against its base node. In a skirmish or multiplayer game, a node blocked more than `MaximumBuildingPlacementFailures` times is removed from the plan, and its structure is placed by the placement search. Before, the house retried a blocked node every `PlacementDelay` minutes without limit. The default is 5 when the rules omit the key. The save revision moves to 32, so saves from earlier builds are not compatible.
