---
title: Keep releases over the modern panel from ordering or selecting
category: fix
release: 0.2.0
targets:
- type: system
  id: sidebar
  effect: changed
- type: system
  id: band-selection
  effect: changed
credit: [MentalHomiega]
---

In the modern interface, a click that starts on the map and is released off the open map, such as over the command bar or the sidebar panel, no longer orders the selected units to move and no longer selects the unit under the release point. Before this change, the release ordered a move to the map cell under the panel.

A rubber band dragged from the map over the panel now stops at the panel's left edge, so it no longer selects units the panel covers. The band still finishes when the button is released over the panel. The same limit applies below the panel: a band never reaches the panel's columns, so units on the open map directly below the panel are not picked by such a band.

Releases over the open map, and the classic interface, are unchanged.
