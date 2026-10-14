---
title: Widen the type-select key to the whole map on repeated taps
category: fix
release: 0.2.0
targets:
- type: command
  id: TypeSelect
  effect: changed
credit:
- MentalHomiega
---

A tap of the type-select key selects the matching units on screen first. When none of them is left unselected, the tap selects every matching unit the player owns on the map, and later taps do the same until a unit is selected or the selection is cleared. Before this change, the key selected only the units on screen.

A unit matches when its type is the type of a selected unit, or the type that such a unit deploys into or undeploys from. Before this change, the key matched on the `GroupAs` name where a type set one and on the type name otherwise, so a deploying unit did not match the structure it deploys into. [`SelectType`](/commands/selecttype/) still matches by `GroupAs`.

A tap posts the game's message for its result: nothing selected, selected across the screen, or selected across the map.
