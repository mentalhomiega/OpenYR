---
title: Keep a vehicle waiting for a busy repair depot in sync when no cell is free nearby
category: fix
release: 0.2.0
targets:
- type: system
  id: repair
  effect: changed
- type: system
  id: network-synchronization
  effect: changed
breaking: false
credit:
- MentalHomiega
---

A vehicle that waits for a busy Service Depot now goes idle where it is when no clear cell is free near the depot, as in Yuri's Revenge. Before, it was sent toward the map's off-map cell. Where that cell lay depended on what each player's screen showed, so the vehicle's turret and its shots could differ between the machines of a network game and between a replay and its recording with the camera moved.
