---
title: Disguise spies as enemy soldiers
category: feature
release: 0.2.0
targets:
- type: system
  id: disguises
  effect: added
- type: key
  id: MakesDisguise
  effect: added
- type: key
  id: CanDisguise
  effect: added
- type: key
  id: FireOnce
  effect: added
- type: key
  id: Range
  scope: weapontype
  effect: changed
credit: [MentalHomiega]
---

A `CanDisguise=yes` object now takes on the look of a soldier it hits with a `MakesDisguise=yes` warhead, as the Yuri's Revenge spy does. `Range=-2` weapons reach any target, and `FireOnce` weapons drop their target after a shot.
