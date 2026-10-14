---
title: Keep a sinking object off the radar
category: fix
release: 0.2.0
targets:
- type: system
  id: cloaking
  effect: changed
credit:
- MentalHomiega
---

A sinking object is no longer plotted on the radar, for any house. Before, a sinking ship the local player owned stayed plotted until it went under, and an enemy ship that was sinking stayed plotted while in plain view. An object that is no longer alive is never plotted either. Radar tests for structures are unchanged.
