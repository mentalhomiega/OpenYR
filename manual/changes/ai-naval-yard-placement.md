---
title: Place a computer naval yard on water near its base
category: fix
release: 0.2.0
targets:
- type: system
  id: ai-base-building
  effect: changed
- type: key
  id: AINavalYardAdjacency
  effect: added
- type: key
  id: Shipyard
  effect: added
credit:
- MentalHomiega
---

A computer house now places a naval yard, or any other naval structure, on open water within `AINavalYardAdjacency` cells of its construction yard, using the foundation of the first `Shipyard` type it may own to size the site. Before, it searched the land around its base, so a naval yard could never be placed and the house gave up on naval structures after the first try. Saves from earlier builds are not compatible.
