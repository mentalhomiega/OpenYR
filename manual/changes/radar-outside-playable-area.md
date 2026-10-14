---
title: Keep objects outside the playable area off the radar
category: fix
release: 0.2.0
targets:
- type: system
  id: cloaking
  effect: changed
credit:
- MentalHomiega
---

An object that is not the local player's is no longer plotted on the radar when the cell under its centre lies outside the playable area set by [`LocalSize`](/keys/localsize/). Before, such an object was plotted when it was in plain view or sensed, so a campaign map's enemy units and structures outside its playable area stayed on the radar. Objects the local player controls, and objects with [`RadarVisible=yes`](/keys/radarvisible/), are unchanged.
