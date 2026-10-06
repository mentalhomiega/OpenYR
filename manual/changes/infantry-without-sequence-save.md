---
title: Save games whose rules have infantry without an animation sequence
category: fix
release: 0.2.0
targets:
- type: key
  id: Sequence
  effect: changed
credit:
- MentalHomiega
---

Saving no longer crashes when an infantry type's art names no `Sequence=`. Such a type now has empty animation controls, which the save, the multiplayer sync check and drawing read like any other type's. Saving a game on the shipped Yuri's Revenge rules crashed this way.
