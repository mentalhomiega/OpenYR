---
title: Destroyed structures handle every unit in radio contact
category: fix
release: 0.2.0
targets:
- type: system
  id: destruction-and-debris
  effect: changed
credit:
- MentalHomiega
---

A destroyed structure now handles every unit in radio contact with it, not only the first. A unit within a cell of the structure, or any unit on a helipad, takes a C4 blast of ten times its maximum strength, as the original game does. Other units are told to move off. Before, only the first unit in contact was handled.
