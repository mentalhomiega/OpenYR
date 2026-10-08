---
title: Search shrapnel cells in the game's cell spread order
category: fix
release: 0.2.0
targets:
- type: key
  id: ShrapnelWeapon
  effect: changed
credit: [MentalHomiega]
---

We search the cells around a shrapnel impact in the game's cell spread order, from ring 1 out to the weapon's range. Before, we searched square rings that also included the impact cell, so a fragment could hit the object the shell struck. The same table orders the cells that blasts and other area searches visit. Within each ring the order now matches the game, and ring 11 covers the cells the game's table lists.
