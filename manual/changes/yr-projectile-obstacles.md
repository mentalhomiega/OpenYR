---
title: Stop projectiles at walls and cliffs as Yuri's Revenge does
category: feature
release: 0.2.0
targets:
- type: key
  id: SubjectToWalls
  effect: added
- type: key
  id: SubjectToCliffs
  effect: added
- type: key
  id: AlliedWallTransparency
  effect: added
- type: key
  id: High
  scope: bullettype
  effect: changed
- type: key
  id: High
  scope: overlaytype
  effect: changed
- type: system
  id: walls-and-gates
  effect: changed
credit: [Lucas]
---

Projectiles now stop at walls and cliffs according to their `SubjectToWalls` and `SubjectToCliffs` settings, as in Yuri's Revenge, so a tank's shells hit a wall in their path while artillery flies over it. The `High` keys no longer affect projectiles.
