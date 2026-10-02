---
title: Accept the Water, WaterBeach and CrusherAll movement zones
category: fix
release: 0.2.0
targets:
- type: enum
  id: MZoneType
  effect: changed
credit: [Lucas]
---

Ships and the Battle Fortress no longer crash the game when they plan a route. Their `MovementZone` values, `Water`, `WaterBeach` and `CrusherAll`, are now recognized; `WaterBeach` follows the same cells as `Water`, and `CrusherAll` the same cells as `Crusher`.
