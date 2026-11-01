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

A rubber band dragged from the map over the panel no longer selects the units the panel covers. Units on the open map below the panel are picked by a band that reaches them, even one that also crosses the panel's rows. The band still finishes when the button is released over the panel.

Releases over the open map, and the classic interface, are unchanged.
