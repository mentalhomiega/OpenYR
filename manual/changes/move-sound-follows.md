---
title: Make move sounds follow the moving object
category: fix
release: 0.2.0
targets:
- type: key
  id: MoveSound
  effect: changed
credit:
- MentalHomiega
---

A `MoveSound` loop now moves with its object. While the object moves, the loop is re-aimed at the object's place on every game update, so its pan and volume follow the object. After the object stops, the loop plays on for three updates and ends on the fourth, or ends at once when the object falls or crashes. A sound that ends while the object is still moving starts again. A loop still playing when the object is removed stops with it. Before, the loop stayed at the place where the move began and kept playing after the object was gone.
