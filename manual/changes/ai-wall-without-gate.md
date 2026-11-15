---
title: Plan a computer wall without a gate or wall type the country may own
category: fix
release: 0.2.0
targets:
- type: key
  id: NSGates
  effect: changed
- type: key
  id: EWGates
  effect: changed
- type: key
  id: ConcreteWalls
  effect: changed
credit:
- MentalHomiega
---

A computer house that plans a perimeter wall no longer crashes the game when [`NSGates`](/keys/nsgates/) or [`EWGates`](/keys/ewgates/) lists no gate its country may own, or [`ConcreteWalls`](/keys/concretewalls/) lists no wall it may own. Without a gate the run keeps a three-cell gap, and without a wall the house plans none.
