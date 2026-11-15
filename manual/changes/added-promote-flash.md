---
title: Let promoted objects flash per rank and type (VeteranFlashTimer, Promote.VeteranFlash, Promote.EliteFlash)
category: feature
release: 0.2.0
targets:
- type: key
  id: VeteranFlashTimer
  effect: added
- type: key
  id: Promote.VeteranFlash
  effect: added
- type: key
  id: Promote.EliteFlash
  effect: added
credit: [MentalHomiega]
---

A new veteran now flashes for `[AudioVisual] VeteranFlashTimer` frames, and a type's `Promote.VeteranFlash` and `Promote.EliteFlash` replace the veteran and elite flash lengths for that type, as Ares does. Rules that leave the keys out are unchanged: veterans do not flash and elite objects flash for `EliteFlashTimer`.
