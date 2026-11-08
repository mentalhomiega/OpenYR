---
title: Teleporters choose their landing spot as Yuri's Revenge does
category: fix
release: 0.2.0
targets:
- type: key
  id: Teleporter
  effect: changed
credit:
- MentalHomiega
---

A jump now picks its landing spot when it is ordered. A teleporting vehicle lands in the middle of the cell it is sent to, or in the middle of the cell a nearby search picks when it cannot enter that one. A teleporting soldier lands on the free spot of the cell that is nearest the order, and a soldier sent to a full cell does not jump at all unless its order has a unit or structure as its target. A soldier standing on the cell is no longer removed. Before, the jump took the nearest cell that was clear to move, kept the offset inside the cell and deleted a soldier standing on the exact spot.
