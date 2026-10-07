---
title: Read HoverAttack
category: feature
release: 0.2.0
targets:
- type: key
  id: HoverAttack
  effect: added
credit: [MentalHomiega]
---

`HoverAttack=` is now read from a unit or infantry type. A unit of that type that stands on the ground when it is ordered to attack, or when it is on guard and has a target, now lifts off to fight from the air, like the Rocketeer. It defaults to `no`.

Saved games made by earlier builds are refused, because a type now stores the flag.
