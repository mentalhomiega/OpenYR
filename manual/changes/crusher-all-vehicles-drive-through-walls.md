---
title: Let a CrusherAll vehicle drive through any wall
category: fix
release: 0.2.0
targets:
- type: enum
  id: MZoneType
  effect: changed
credit:
- MentalHomiega
---

A vehicle with `MovementZone=CrusherAll`, such as the Battle Fortress, now rolls through every wall, as in Yuri's Revenge, and crushes it on the way. Before, only a wall marked `Crushable=yes` let it through. It drove round a concrete wall, or stopped to shoot at it.
