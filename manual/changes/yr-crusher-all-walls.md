---
title: Let CrusherAll units move onto crushable walls
category: fix
release: 0.2.0
targets:
- type: enum
  id: MZoneType
  effect: changed
credit: [MentalHomiega]
---

A unit with `MovementZone=CrusherAll`, such as the Battle Fortress, now counts a cell with a crushable wall as clear to move into, as a `Crusher` unit does. Before, its routes crossed such walls but the cell check refused them.
