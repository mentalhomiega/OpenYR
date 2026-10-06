---
title: Mark objects hidden behind tall structures
category: feature
release: 0.2.0
targets:
- type: key
  id: CanHideThings
  effect: added
- type: key
  id: CanBeHidden
  effect: added
- type: key
  id: Behind
  effect: added
- type: key
  id: ShowHidden
  effect: added
credit: [MentalHomiega]
---

Structures with `CanHideThings` now cover the cells behind them, set by `OccupyHeight`, `AddOccupy` and `RemoveOccupy`. An object standing in a covered cell is marked with the `Behind` animation while `ShowHidden` is on, or with blinking brackets when no `Behind` animation is set.
