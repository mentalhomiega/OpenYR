---
title: Turn ice behavior off in a theater whose ice tile sets are empty
category: fix
release: 0.2.0
targets:
- type: key
  id: IsIceGrowthEnabled
  scope: theater
  effect: changed
- type: key
  id: Ice1Set
  effect: changed
credit:
- MentalHomiega
---

A snow theater whose ice tile sets are missing or hold no tiles no longer runs the ice passes. Before, the first ice growth pass of a map that allows ice growth read the tiles after the empty sets as ice and rewrote thousands of ramp and ground cells, which left black wedges in the terrain. The debug log now says when a theater's ice behavior is turned off for this reason.
