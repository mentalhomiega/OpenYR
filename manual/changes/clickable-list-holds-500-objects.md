---
title: Limit the clickable list to 500 objects
category: fix
release: 0.2.0
targets:
- type: system
  id: band-selection
  effect: changed
credit:
- MentalHomiega
---

The clickable list holds at most 500 objects. In a frame where more than 500 objects are drawn, the objects drawn after the first 500 cannot be selected by a click or a box, and the type-select key does not select them on screen. Before this change, the list had no limit, so every object drawn in view could be selected.
