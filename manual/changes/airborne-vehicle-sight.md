---
title: Let an airborne vehicle look around while it flies
category: fix
release: 0.2.0
targets:
- type: system
  id: map-visibility
  effect: changed
credit:
- MentalHomiega
---

A jump-jet vehicle such as the Kirov now reveals the shroud around itself while it flies, every 15 frames, as aircraft do. Before, it looked only when a move ended or it landed, so it cleared nothing along its route. An aircraft or airborne vehicle now looks only while it moves and only for the player or an ally of the player.
