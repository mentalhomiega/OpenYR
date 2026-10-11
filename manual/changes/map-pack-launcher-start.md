---
title: Start maps held in map packs from a launch file
category: fix
release: 0.2.0
targets:
- type: format
  id: map-packs
  effect: changed
credit:
- MentalHomiega
---

A launch file can now start a map that only a `.YRO` map pack holds. Before, the game read the launch file's scenario before it mounted the packs, so the map was not found and the game stopped. Starting the same map from the skirmish list already worked.
