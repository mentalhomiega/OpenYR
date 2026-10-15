---
title: Draw the Floating Disc's laser when it attacks the ground
category: fix
release: 0.2.0
targets:
- type: key
  id: DiskLaser
  effect: changed
credit: [MentalHomiega]
---

A `DiskLaser=yes` weapon now draws its ring and strikes when it attacks a cell, as it does against an object. Before, a ground attack fired an ordinary invisible bullet at the cell, so no ring was drawn and the damage did not wait for the ring to close.
