---
title: Run Yuri's Revenge action 126, Restore Starting Units
category: feature
release: 0.2.0
targets:
- type: action
  id: TACTION_RESTORE_STARTING_UNITS
  effect: changed
- type: action
  id: TACTION_RESTORE_STARTING_BUILDINGS
  effect: changed
credit:
- MentalHomiega
---

Action 126 puts back every structure, vehicle, soldier and aircraft a house had on the map when the scenario finished loading. It did nothing before.

Restore Starting Buildings now reads the same list, so a structure a house held at the end of loading counts as a starting structure even when the map's `[Structures]` section did not place it. A save keeps the list, so the save revision went up.
