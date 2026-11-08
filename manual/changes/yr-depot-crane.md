---
title: Keep vehicles off a repair structure's blocked columns
category: fix
release: 0.2.0
targets:
- type: system
  id: movement-and-terrain
  effect: changed
- type: key
  id: NumberImpassableRows
  effect: changed
credit: [MentalHomiega]
---

A vehicle in radio contact with a `UnitRepair=yes` structure, such as a service depot, no longer drives onto the columns that `NumberImpassableRows` counts. Before this change, a damaged vehicle that docked at the depot could stop on its crane column, and a step from one cell of the depot onto another was allowed for any order.

A vehicle on its ENTER mission may still drive over those columns, so the bay can still be reached and repaired.
