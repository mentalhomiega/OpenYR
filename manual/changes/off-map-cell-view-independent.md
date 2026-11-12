---
title: Keep the game in sync when a unit heads for a cell outside the map
category: fix
release: 0.2.0
targets:
- type: system
  id: network-synchronization
  effect: changed
breaking: false
credit:
- MentalHomiega
---

A unit sent toward a cell outside the map now heads for the same place on every machine of a network game, and in a replay whatever the camera shows. Every cell outside the map shares one stand-in cell, and drawing the map near its edge moved that cell to the place drawn last. A unit whose destination or target was that cell followed each player's view, so its turret and its shots could differ between machines. Yuri's Revenge has the same shared cell and the same dependence on the view.
