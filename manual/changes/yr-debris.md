---
title: Throw wreckage as Yuri's Revenge does
category: feature
release: 0.2.0
targets:
- type: key
  id: MaxDebris
  effect: changed
- type: key
  id: MinDebris
  effect: added
- type: key
  id: DebrisAnims
  effect: added
credit: [MentalHomiega]
---

A destroyed object now throws between `MinDebris` and one less than `MaxDebris` pieces. Voxel `DebrisTypes` are dealt out in turn until the count is used up, and the rest goes to the new `DebrisAnims` list. `MetallicDebris` is used only when a type has neither list, and an empty `MetallicDebris` no longer crashes the game when such an object dies.
