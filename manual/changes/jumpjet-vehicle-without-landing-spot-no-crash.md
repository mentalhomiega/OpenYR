---
title: Stop a jump-jet vehicle with no landing spot from crashing the game
category: fix
release: 0.2.0
targets:
- type: system
  id: movement-and-terrain
  effect: changed
credit:
- MentalHomiega
---

A moving jump-jet vehicle, such as the Kirov Airship, that is stopped with no free cell to put down in is destroyed as before. The game used to crash with a stack overflow at that moment; it now finishes destroying the vehicle.
