---
title: Add the ExtendedAnimFacings key
category: feature
release: 0.2.0
targets:
- type: key
  id: ExtendedAnimFacings
  effect: added
credit: [MentalHomiega]
---

`ExtendedAnimFacings=yes` in a weapon's section lets a list of 16, 32 or 64 `Anim` entries pick its entry by the direction of the shot. The default, `no`, matches Yuri's Revenge. A save made before this build may not load, because the weapon data in a save gained this field.
