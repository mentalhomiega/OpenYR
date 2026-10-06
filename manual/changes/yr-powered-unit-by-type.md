---
title: Power each PoweredUnit type only from its own control structure
category: fix
release: 0.2.0
targets:
- type: key
  id: PoweredUnit
  effect: changed
- type: key
  id: PowersUnit
  effect: changed
credit: [MentalHomiega]
---

A `PoweredUnit=yes` unit now keeps running only while its owner has a working structure whose `PowersUnit` names the unit's type. Before, any working structure with a `PowersUnit` type kept every powered unit type of its owner running.
