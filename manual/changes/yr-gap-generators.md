---
title: Shroud the ground around gap generators
category: feature
release: 0.2.0
targets:
- type: system
  id: map-visibility
  effect: changed
- type: key
  id: GapGenerator
  effect: added
- type: key
  id: GapRadiusInCells
  effect: added
credit: [Lucas]
---

A working `GapGenerator=yes` structure now shrouds the ground around it for players who are not its owner's allies, as the Yuri's Revenge gap generator does.
