---
title: Draw the target radius of an aimed superweapon, and show the no-shield cursor away from allied structures
category: feature
release: 0.2.0
targets:
- type: system
  id: superweapons
  effect: changed
credit:
- MentalHomiega
---

While a superweapon is aimed, the map draws its `Range=` as rings around the cell under the pointer, as Yuri's Revenge does. `LineMultiplier=` sets how many rings are drawn across that radius. The rings show only while unit action lines are on.

A force shield's cursor is its own only over a structure of the player or an ally. Anywhere else the pointer shows the no-shield cursor.
