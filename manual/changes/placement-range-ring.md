---
title: Show the range ring of a structure being placed
category: feature
release: 0.2.0
targets:
- type: system
  id: action-lines
  effect: changed
- type: key
  id: UnitActionLines
  effect: changed
credit: [MentalHomiega]
---

A structure waiting to be placed now draws its range as a ring around the cursor cell, as the original game does. The ring shows while `UnitActionLines=yes`. A defense shows its primary weapon's range. A psychic detector, gap generator, sensor array or cloak generator shows its own radius. The player's nearest structures of the same type also draw their rings when they lie within twice that range of the cursor.

Before this change, placing a defense showed no range.
