---
title: Action house number 8997 names the triggering house
category: fix
release: 0.2.0
targets:
- type: action
  id: TACTION_MIND_CONTROL_BASE
  effect: changed
- type: action
  id: TACTION_RESTORE_MIND_CONTROLLED_BASE
  effect: changed
- type: action
  id: TACTION_RESTORE_STARTING_UNITS
  effect: changed
- type: action
  id: TACTION_CHANGE_HOUSE
  effect: changed
credit: [MentalHomiega]
---

A trigger action that takes a house by number now reads `8997` as Yuri's Revenge does: the house that owns the object that last satisfied one of the trigger's entry, zone or line-crossing events. Before, most actions found no house for it, and the three actions that read it took the trigger's own house instead. A save keeps the house. The [scenario objects page](/formats/scenario-objects/#spawn-houses) lists the actions.
