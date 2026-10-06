---
title: Match the Yuri's Revenge weapon range check
category: fix
release: 0.2.0
targets:
- type: key
  id: Range
  effect: changed
- type: key
  id: AirRangeBonus
  effect: added
- type: key
  id: Arcing
  effect: changed
credit: [MentalHomiega]
---

Weapons reached a third of a cell less than their `Range`, arcing weapons ignored `Range` and fired at any target their arc could reach, and `AirRangeBonus` was ignored. Weapons now reach their full `Range`, an arcing weapon must also have its target within `Range` (with the elevation bonus in whole cells), and `AirRangeBonus` adds reach against targets in the air, so the IFV reaches 4 cells farther against aircraft.
