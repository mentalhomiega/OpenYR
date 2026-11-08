---
title: Release a war factory's vehicles at its ExitCoord
category: fix
release: 0.2.0
targets:
- type: key
  id: ExitCoord
  effect: changed
- type: system
  id: production
  effect: changed
credit:
- MentalHomiega
---

A [`WeaponsFactory=yes`](/keys/weaponsfactory/) structure now releases each object at the point its [`ExitCoord`](/keys/exitcoord/) names, measured from the structure's location. Before, it used a fixed point near the structure's west end, so a vehicle from a factory at 60,20 appeared at cell 60,21 instead of 62,21. A structure without the key releases at the offset its foundation gives, which is the value the stock war factories write.

A jump-jet vehicle or Floating Disc flies to the door cell and then heads for the rally point. Before, it flew to the cell one step west of the door cell. A vehicle that drives out still follows its door route, and its rally point is unchanged.
