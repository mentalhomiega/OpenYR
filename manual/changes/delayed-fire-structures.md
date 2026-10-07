---
title: Charge a Tesla coil's shot with its special animation
category: feature
release: 0.2.0
targets:
- type: key
  id: IsAnimDelayedFire
  effect: added
- type: key
  id: DelayedFireDelay
  effect: changed
credit: [MentalHomiega]
---

`IsAnimDelayedFire=` is now read from a structure's art section. A structure with it set to `yes`, such as the Tesla coil, plays its `SpecialAnim` in place of its `ActiveAnim` for `DelayedFireDelay=` frames before each shot, then fires and shows the `ActiveAnim` again. A prism tower's `SpecialAnim` now plays when it charges, and no longer plays each time its house regains power. Saved games made by earlier builds are refused, because a type now stores the setting.
