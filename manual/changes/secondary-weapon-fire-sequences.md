---
title: Fire the second weapon with its own sequences
category: fix
release: 0.2.0
targets:
- type: key
  id: SecondaryFire
  effect: added
- type: key
  id: SecondaryProne
  effect: added
- type: key
  id: Sequence
  effect: changed
credit: [MentalHomiega]
---

A soldier that fires its second weapon now plays the `SecondaryFire` sequence standing and `SecondaryProne` lying down, where it used to play `FireUp` and `FireProne` for both weapons. Boris and the Brute show their second attacks. A type whose art has no such sequence keeps the first weapon's firing animation. A dug-in or swimming soldier still fights with its deployed or wet sequence.

Saved games made by earlier builds are refused, because a type now stores the release stage of its second weapon.
