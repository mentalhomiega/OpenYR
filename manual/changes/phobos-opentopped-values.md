---
title: Set open-topped range, damage and warp distance per transport and per passenger
category: feature
release: 0.2.0
targets:
- type: key
  id: OpenTopped.RangeBonus
  effect: added
- type: key
  id: OpenTopped.DamageMultiplier
  effect: added
- type: key
  id: OpenTopped.WarpDistance
  effect: added
- type: key
  id: OpenTransport.RangeBonus
  effect: added
- type: key
  id: OpenTransport.DamageMultiplier
  effect: added
credit:
- MentalHomiega
---

The Phobos keys `OpenTopped.RangeBonus`, `OpenTopped.DamageMultiplier` and `OpenTopped.WarpDistance` set an open-topped transport's own values in place of the `[CombatDamage]` ones. `OpenTransport.RangeBonus` and `OpenTransport.DamageMultiplier` add a passenger's own bonus and multiplier on top of the transport's, with `[CombatDamage]` defaults of 0 and 1.0. Without these keys, nothing changes.
